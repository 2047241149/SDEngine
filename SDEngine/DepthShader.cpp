#include"DepthShader.h"

DepthShader::DepthShader(WCHAR* vsFilenPath, WCHAR* psFilenPath)
{
	Initialize(vsFilenPath, psFilenPath);
}


DepthShader::DepthShader(const DepthShader& other)
{

}


DepthShader::~DepthShader()
{
	ShutDown();
}


bool DepthShader::Initialize(WCHAR* vsFilenPath, WCHAR* psFilenPath)
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



bool DepthShader::SetShaderParams(ID3D11ShaderResourceView* diffuseTexture)
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




bool DepthShader::InitializeShader(WCHAR* VSFileName, WCHAR* PSFileName)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* VertexShaderBuffer;
	ID3D10Blob* PixelShaderBuffer;

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
			Log::LogShaderCompileInfo(errorMessage, VSFileName);
		}
		//不存在错误信息,也就是没有找到Shader文件
		else
		{
			MessageBox(NULL, L"can not find VS file", L"error", MB_OK);
		}
	}

	HR(g_pDevice->CreateVertexShader(VertexShaderBuffer->GetBufferPointer(), VertexShaderBuffer->GetBufferSize(), NULL, &md3dVertexShader));


	//第三,编译PixelShader,并创建PixelShader
	result = D3DCompileFromFile(PSFileName, NULL, NULL, "PS", "ps_5_0", flag, 0, &PixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		//存在错误信息
		if (errorMessage)
		{
			Log::LogShaderCompileInfo(errorMessage, PSFileName);
		}
		//不存在错误信息,也就是没有找到Shader文件
		else
		{
			MessageBox(NULL, L"can not find PS file", L"error", MB_OK);
		}
	}

	HR(g_pDevice->CreatePixelShader(PixelShaderBuffer->GetBufferPointer(), PixelShaderBuffer->GetBufferSize(), NULL, &md3dPixelShader));

	//第四,填充输入布局形容结构体,创建输入布局
	D3D11_INPUT_ELEMENT_DESC VertexInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // 96位即12个字节
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};

	unsigned int numElements = sizeof(VertexInputLayout) / sizeof(VertexInputLayout[0]);         //布局数量

	HR(g_pDevice->CreateInputLayout(VertexInputLayout, numElements, VertexShaderBuffer->GetBufferPointer(), VertexShaderBuffer->GetBufferSize(), &md3dInputLayout));

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

	HR(g_pDevice->CreateBuffer(&matrixBufferDesc, NULL, &mCBMatrixBuffer));

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

	HR(g_pDevice->CreateSamplerState(&samplerDesc, &mSamplerState));

	return true;
}



void DepthShader::ShutDown()
{
	ReleaseCOM(mCBMatrixBuffer);
	ReleaseCOM(md3dInputLayout);
	ReleaseCOM(md3dPixelShader);
	ReleaseCOM(md3dVertexShader);
}


bool DepthShader::SetShaderCB(ID3D11ShaderResourceView* diffuseTexture)
{

	XMMATRIX uiViewMatrix = GCamera->GetUIViewMatrix();
	XMMATRIX uiOrhoMatrix = GCamera->GetUIOrthoMatrix();
	//第一，更新变换矩阵常量缓存的值
	//将矩阵转置,在传入常量缓存前进行转置,因为GPU对矩阵数据会自动进行一次转置
	D3D11_MAPPED_SUBRESOURCE mappedSubresource1;
	HR(g_pDeviceContext->Map(mCBMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource1));
	auto pCBMatrix = reinterpret_cast<CBMatrix*>(mappedSubresource1.pData);
	XMMATRIX uiViewMa = XMMatrixTranspose(uiViewMatrix);
	XMMATRIX uiOrthoMa = XMMatrixTranspose(uiOrhoMatrix);
	pCBMatrix->mUIViewMatrix = uiViewMa;
	pCBMatrix->mUIOrthoMatrix = uiOrthoMa;
	pCBMatrix->farPlane =GCamera->GetFarPlane();
	pCBMatrix->nearPlane = GCamera->GetNearPlane();
	pCBMatrix->pad = XMFLOAT2(0.0f, 0.0f);
	g_pDeviceContext->Unmap(mCBMatrixBuffer, 0);


	//第三,设置在VertexShader的常量缓存的值(带着更新的值)
	g_pDeviceContext->VSSetConstantBuffers(0, 1, &mCBMatrixBuffer);
	g_pDeviceContext->PSSetConstantBuffers(0, 1, &mCBMatrixBuffer);
	g_pDeviceContext->PSSetShaderResources(0, 1, &diffuseTexture);

	return true;
}

bool DepthShader::SetShaderState()
{

	
	//设置顶点输入布局
	g_pDeviceContext->IASetInputLayout(md3dInputLayout);

	//设置VertexShader和PixelShader
	g_pDeviceContext->VSSetShader(md3dVertexShader, NULL, 0);
	g_pDeviceContext->PSSetShader(md3dPixelShader, NULL, 0);

	//设置采样状态
	g_pDeviceContext->PSSetSamplers(0, 1, &mSamplerState);  //S0注册 对应0

	return true;
}


