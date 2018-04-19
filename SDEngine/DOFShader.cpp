#include "DOFShader.h"


DOFShader::DOFShader(WCHAR* vsFilenPath, WCHAR* psFilenPath)
{
	Initialize(vsFilenPath, psFilenPath);
}


DOFShader::DOFShader(const DOFShader& other)
{

}


DOFShader::~DOFShader()
{
	ShutDown();
}


bool DOFShader::Initialize(WCHAR* vsFilenPath, WCHAR* psFilenPath)
{
	//置空指针
	md3dVertexShader = NULL;
	md3dPixelShader = NULL;
	md3dInputLayout = NULL;
	mCBDOF = NULL;
	bool result;
	result = InitializeShader(vsFilenPath, psFilenPath);
	if (!result)
		return false;

	return true;
}



bool DOFShader::SetShaderParams(ID3D11ShaderResourceView* screenRT, ID3D11ShaderResourceView* screenBlurRT,
	ID3D11ShaderResourceView* depthRT,
	float dofStart, float dofRange,float farPlane, float nearPlane)
{
	bool result;
	//设置Shader常量缓存和纹理资源
	result = SetShaderCB(screenRT,screenBlurRT, depthRT,dofStart,dofRange,farPlane,nearPlane);
	if (!result)
		return false;

	//设置VertexShader PixelShader InputLayout SamplerState
	SetShaderState();

	return true;
}




bool DOFShader::InitializeShader(WCHAR* VSFileName, WCHAR* PSFileName)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* VertexShaderBuffer;
	ID3D10Blob* PixelShaderBuffer;
	ID3D11Device* d3dDevice = D3DClass::GetInstance()->GetDevice();
	ID3D11DeviceContext* d3dDeviceContext = D3DClass::GetInstance()->GetDeviceContext();


	//第一,初始化参数
	errorMessage = NULL;
	VertexShaderBuffer = NULL;
	PixelShaderBuffer = NULL;
	DWORD flag = D3DCOMPILE_ENABLE_STRICTNESS;

	#if _DEBUG
	flag |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	#endif

	//第二,编译VertexShader代码,并创建VertexShader
	result = D3DCompileFromFile(VSFileName, NULL, NULL, "VS", "vs_5_0", flag, 0, &VertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		//存在错误信息
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage,VSFileName);
		}
		//不存在错误信息,也就是没有找到Shader文件
		else
		{
			MessageBox(NULL, L"can not find VS file", L"error", MB_OK);
		}
	}

	HR(d3dDevice->CreateVertexShader(VertexShaderBuffer->GetBufferPointer(), VertexShaderBuffer->GetBufferSize(), NULL, &md3dVertexShader));


	//第三,编译PixelShader,并创建PixelShader
	result = D3DCompileFromFile(PSFileName, NULL, NULL, "PS", "ps_5_0", flag, 0, &PixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		//存在错误信息
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, PSFileName);
		}
		//不存在错误信息,也就是没有找到Shader文件
		else
		{
			MessageBox(NULL, L"can not find PS file", L"error", MB_OK);
		}
	}

	HR(d3dDevice->CreatePixelShader(PixelShaderBuffer->GetBufferPointer(), PixelShaderBuffer->GetBufferSize(), NULL, &md3dPixelShader));

	//第四,填充输入布局形容结构体,创建输入布局
	D3D11_INPUT_ELEMENT_DESC VertexInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		
	};

	unsigned int numElements = sizeof(VertexInputLayout) / sizeof(VertexInputLayout[0]);         //布局数量

	HR(d3dDevice->CreateInputLayout(VertexInputLayout, numElements, VertexShaderBuffer->GetBufferPointer(), VertexShaderBuffer->GetBufferSize(), &md3dInputLayout));

	//第五,释放VertexShaderBuffer和PixelShaderBuffer
	VertexShaderBuffer->Release();
	VertexShaderBuffer = NULL;
	PixelShaderBuffer->Release();
	PixelShaderBuffer = NULL;

	D3D11_BUFFER_DESC cBufferDesc;
	ZeroMemory(&cBufferDesc, sizeof(cBufferDesc));
	cBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cBufferDesc.ByteWidth = sizeof(CBDOF);   //结构体大小,必须为16字节倍数
	cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HR(d3dDevice->CreateBuffer(&cBufferDesc, NULL, &mCBDOF));

	//第七,填充采样形容结构体,并且创建采样状态
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;  //都是线性插值(三种方式,点过滤,线性过滤,异性过滤)
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HR(d3dDevice->CreateSamplerState(&samplerDesc, &mSamplerLinearWrap));

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	HR(d3dDevice->CreateSamplerState(&samplerDesc, &mSamplerLinearClamp));

	return true;
}



void DOFShader::ShutDown()
{
	ReleaseCOM(mCBDOF);
	ReleaseCOM(md3dInputLayout);
	ReleaseCOM(md3dPixelShader);
	ReleaseCOM(md3dVertexShader);
}

void DOFShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	// 获取指向错误信息文本的指针
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// 获取错误信息文本的长度
	bufferSize = errorMessage->GetBufferSize();

	// 创建一个txt,用于写入错误信息
	fout.open("shader-error.txt");

	//想txt文件写入错误信息
	for (i = 0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// 关闭文件
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	//弹出提醒的小窗口
	MessageBox(NULL, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

}


bool DOFShader::SetShaderCB(ID3D11ShaderResourceView* screenRT, ID3D11ShaderResourceView* screenBlurRT,
	ID3D11ShaderResourceView* depthRT,
	float dofStart, float dofRange, float farPlane, float nearPlane)
{

	ID3D11DeviceContext* d3dDeviceContext = D3DClass::GetInstance()->GetDeviceContext();

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	HR(d3dDeviceContext->Map(mCBDOF, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource));
	auto pCBDOF = reinterpret_cast<CBDOF*>(mappedSubresource.pData);
	pCBDOF->depthRange = dofRange;
	pCBDOF->depthStart = dofStart;
	pCBDOF->farPlane = farPlane;
	pCBDOF->nearPlane = nearPlane;
	d3dDeviceContext->Unmap(mCBDOF, 0);

	d3dDeviceContext->PSSetConstantBuffers(0, 1, &mCBDOF);
	d3dDeviceContext->PSSetShaderResources(0, 1, &screenRT);
	d3dDeviceContext->PSSetShaderResources(1, 1, &screenBlurRT);
	d3dDeviceContext->PSSetShaderResources(2, 1, &depthRT);

	return true;
}

bool DOFShader::SetShaderState()
{

	ID3D11DeviceContext* deviceContext = D3DClass::GetInstance()->GetDeviceContext();

	//设置顶点输入布局
	deviceContext->IASetInputLayout(md3dInputLayout);

	//设置VertexShader和PixelShader
	deviceContext->VSSetShader(md3dVertexShader, NULL, 0);
	deviceContext->PSSetShader(md3dPixelShader, NULL, 0);

	//设置采样状态
	deviceContext->PSSetSamplers(0, 1, &mSamplerLinearWrap); 
	deviceContext->PSSetSamplers(1, 1, &mSamplerLinearClamp);

	return true;
}


