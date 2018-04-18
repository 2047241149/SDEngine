#include"UIShader.h"

UIShader::UIShader(WCHAR* vsFilenPath, WCHAR* psFilenPath)
{
	Initialize(vsFilenPath, psFilenPath);
}


UIShader::UIShader(const UIShader& other)
{

}


UIShader::~UIShader()
{
	ShutDown();
}


bool UIShader::Initialize(WCHAR* vsFilenPath, WCHAR* psFilenPath)
{
	//置空指针
	md3dVertexShader = NULL;
	md3dPixelShader = NULL;
	md3dInputLayout = NULL;
	mCBMatrixBuffer = NULL;

	bool result;
	result = InitializeShader(vsFilenPath, psFilenPath);
	if (!result)
		return false;

	return true;
}



bool UIShader::SetShaderParams(ID3D11ShaderResourceView* diffuseTexture)
{
	bool result;

	//设置Shader常量缓存和纹理资源
	result = SetShaderCB(diffuseTexture);
	if (!result)
		return false;

	//设置VertexShader PixelShader InputLayout SamplerState
	SetShaderState();

	return true;
}




bool UIShader::InitializeShader(WCHAR* VSFileName, WCHAR* PSFileName)
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
			OutputShaderErrorMessage(errorMessage, VSFileName);
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
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // 96位即12个字节
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};

	unsigned int numElements = sizeof(VertexInputLayout) / sizeof(VertexInputLayout[0]);         //布局数量

	HR(d3dDevice->CreateInputLayout(VertexInputLayout, numElements, VertexShaderBuffer->GetBufferPointer(), VertexShaderBuffer->GetBufferSize(), &md3dInputLayout));

	//第五,释放VertexShaderBuffer和PixelShaderBuffer
	VertexShaderBuffer->Release();
	VertexShaderBuffer = NULL;
	PixelShaderBuffer->Release();
	PixelShaderBuffer = NULL;

	//第六,设置(变换矩阵常量)缓存形容结构体,并创建矩阵常量缓存
	D3D11_BUFFER_DESC matrixBufferDesc;
	ZeroMemory(&matrixBufferDesc, sizeof(matrixBufferDesc));
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(CBMatrix);   //结构体大小,必须为16字节倍数
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HR(d3dDevice->CreateBuffer(&matrixBufferDesc, NULL, &mCBMatrixBuffer));

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

	HR(d3dDevice->CreateSamplerState(&samplerDesc, &mSamplerState));

	return true;
}



void UIShader::ShutDown()
{
	ReleaseCOM(mCBMatrixBuffer);
	ReleaseCOM(md3dInputLayout);
	ReleaseCOM(md3dPixelShader);
	ReleaseCOM(md3dVertexShader);
}

void UIShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, WCHAR* shaderFilename)
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


bool UIShader::SetShaderCB(ID3D11ShaderResourceView* diffuseTexture)
{

	ID3D11DeviceContext* d3dDeviceContext = D3DClass::GetInstance()->GetDeviceContext();
	XMMATRIX uiViewMatrix = Camera::GetInstance()->GetUIViewMatrix();
	XMMATRIX uiOrhoMatrix = Camera::GetInstance()->GetUIOrthoMatrix();

	//第一，更新变换矩阵常量缓存的值
	//将矩阵转置,在传入常量缓存前进行转置,因为GPU对矩阵数据会自动进行一次转置
	D3D11_MAPPED_SUBRESOURCE mappedSubresource1;
	HR(d3dDeviceContext->Map(mCBMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource1));
	auto pCBMatrix = reinterpret_cast<CBMatrix*>(mappedSubresource1.pData);
	XMMATRIX uiViewMa = XMMatrixTranspose(uiViewMatrix);
	XMMATRIX uiOrthoMa = XMMatrixTranspose(uiOrhoMatrix);
	pCBMatrix->mUIViewMatrix = uiViewMa;
	pCBMatrix->mUIOrthoMatrix = uiOrthoMa;
	d3dDeviceContext->Unmap(mCBMatrixBuffer, 0);


	//第三,设置在VertexShader的常量缓存的值(带着更新的值)
	d3dDeviceContext->VSSetConstantBuffers(0, 1, &mCBMatrixBuffer);

	d3dDeviceContext->PSSetShaderResources(0, 1, &diffuseTexture);

	return true;
}

bool UIShader::SetShaderState()
{

	ID3D11DeviceContext* deviceContext = D3DClass::GetInstance()->GetDeviceContext();

	//设置顶点输入布局
	deviceContext->IASetInputLayout(md3dInputLayout);

	//设置VertexShader和PixelShader
	deviceContext->VSSetShader(md3dVertexShader, NULL, 0);
	deviceContext->PSSetShader(md3dPixelShader, NULL, 0);

	//设置采样状态
	deviceContext->PSSetSamplers(0, 1, &mSamplerState);  //S0注册 对应0

	return true;
}


