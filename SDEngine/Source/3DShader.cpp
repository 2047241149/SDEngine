#include "3DShader.h"

Shader_3D::Shader_3D(WCHAR* vsFilenPath, WCHAR* psFilenPath):
	m_pVertexShader(nullptr),
	m_pPixelShader(nullptr),
	m_pInputLayout(nullptr),
	m_pCBCommon(nullptr),
	m_pWrapLinearSampler(nullptr),
	m_pClampPointSampler(nullptr)	
{
	Init(vsFilenPath, psFilenPath);
}


Shader_3D::Shader_3D(const Shader_3D& other)
{

}


Shader_3D::~Shader_3D()
{
	ShutDown();
}


bool Shader_3D::Init(WCHAR* vsFilenPath, WCHAR* psFilenPath)
{
	bool result;
	result = InitShader(vsFilenPath, psFilenPath);

	if (!result)
		return false;

	return true;
}



bool Shader_3D::SetShaderParams(CXMMATRIX worldMatrix)
{
	bool result;
	//设置Shader常量缓存和纹理资源
	result = SetShaderCB(worldMatrix);
	if (!result)
		return false;

	//设置VertexShader PixelShader InputLayout SamplerState
	SetShaderState();

	return true;
}

bool Shader_3D::InitShader(WCHAR* VSFileName, WCHAR* PSFileName)
{
	HRESULT result;
	ID3D10Blob* pErrorMessage;
	ID3D10Blob* pVertexShaderBlob;
	ID3D10Blob* pPixelShaderBob;

	//第一,初始化参数
	pErrorMessage = nullptr;
	pVertexShaderBlob = nullptr;
	pPixelShaderBob = nullptr;
	DWORD flag = D3DCOMPILE_ENABLE_STRICTNESS;

	#if _DEBUG
	flag |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	#endif

	//第二,编译VertexShader代码,并创建VertexShader
	result = D3DCompileFromFile(VSFileName, NULL, NULL, "VS", "vs_5_0", flag, 0, &pVertexShaderBlob, &pErrorMessage);
	if (FAILED(result))
	{
		//存在错误信息
		if (pErrorMessage)
		{
			Log::LogShaderCompileInfo(pErrorMessage,VSFileName);
		}
		//不存在错误信息,也就是没有找到Shader文件
		else
		{
			MessageBox(NULL, L"can not find VS file", L"error", MB_OK);
		}
	}

	HR(g_pDevice->CreateVertexShader(pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), NULL, &m_pVertexShader));


	//第三,编译PixelShader,并创建PixelShader
	result = D3DCompileFromFile(PSFileName, NULL, NULL, "PS", "ps_5_0", flag, 0, &pPixelShaderBob, &pErrorMessage);
	if (FAILED(result))
	{
		//存在错误信息
		if (pErrorMessage)
		{
			Log::LogShaderCompileInfo(pErrorMessage, PSFileName);
		}
		//不存在错误信息,也就是没有找到Shader文件
		else
		{
			MessageBox(NULL, L"can not find PS file", L"error", MB_OK);
		}
	}

	HR(g_pDevice->CreatePixelShader(pPixelShaderBob->GetBufferPointer(), pPixelShaderBob->GetBufferSize(), NULL, &m_pPixelShader));

	//第四,填充输入布局形容结构体,创建输入布局
	D3D11_INPUT_ELEMENT_DESC VertexInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		
	};

	//布局数量
	unsigned int numElements = sizeof(VertexInputLayout) / sizeof(VertexInputLayout[0]);         

	HR(g_pDevice->CreateInputLayout(VertexInputLayout, numElements, pVertexShaderBlob->GetBufferPointer(),
		pVertexShaderBlob->GetBufferSize(), &m_pInputLayout));

	//第五,释放pVertexShaderBlob和pPixelShaderBob
	ReleaseCOM(pVertexShaderBlob);
	ReleaseCOM(pPixelShaderBob);

	//第六,设置(变换矩阵常量)缓存形容结构体,并创建矩阵常量缓存
	D3D11_BUFFER_DESC commonBufferDesc;
	ZeroMemory(&commonBufferDesc, sizeof(commonBufferDesc));
	commonBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	commonBufferDesc.ByteWidth = sizeof(CBCommmon);  
	commonBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	commonBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	HR(g_pDevice->CreateBuffer(&commonBufferDesc, NULL, &m_pCBCommon));

	//第七,填充采样形容结构体,并且创建采样状态
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; 
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
	HR(g_pDevice->CreateSamplerState(&samplerDesc, &m_pWrapLinearSampler));

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	HR(g_pDevice->CreateSamplerState(&samplerDesc, &m_pClampPointSampler));

	return true;
}



void Shader_3D::ShutDown()
{
	ReleaseCOM(m_pCBCommon);
	ReleaseCOM(m_pInputLayout);
	ReleaseCOM(m_pPixelShader);
	ReleaseCOM(m_pVertexShader);
	ReleaseCOM(m_pWrapLinearSampler);
	ReleaseCOM(m_pClampPointSampler);
}

bool Shader_3D::SetShaderCB(CXMMATRIX worldMatrix)
{

	XMMATRIX viewMatrix = GCamera->GetViewMatrix();
	XMMATRIX ProjMatrix = GCamera->GetProjectionMatrix();

	//第一，更新变换矩阵常量缓存的值
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	HR(g_pDeviceContext->Map(m_pCBCommon, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource));
	auto pCBCommon = reinterpret_cast<CBCommmon*>(mappedSubresource.pData);
	XMMATRIX worldMa = XMMatrixTranspose(worldMatrix);
	XMMATRIX viewMa = XMMatrixTranspose(viewMatrix);
	XMMATRIX ProjMa = XMMatrixTranspose(ProjMatrix);
	XMMATRIX worldInvTranspose = XMMatrixTranspose(MathTool::GetInvenseTranspose(worldMatrix));
	pCBCommon->mWorldMatrix = worldMa;
	pCBCommon->mViewMatrix = viewMa;
	pCBCommon->mProjMatrix = ProjMa;
	pCBCommon->mWorldInvTranposeMatirx = worldInvTranspose;
	pCBCommon->cameraPos = GCamera->GetPosition();
	g_pDeviceContext->Unmap(m_pCBCommon, 0);

	//第三,设置在VertexShader的常量缓存的值(带着更新的值)
	g_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pCBCommon);
	g_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pCBCommon);

	return true;
}

bool Shader_3D::SetShaderState()
{
	//设置顶点输入布局
	g_pDeviceContext->IASetInputLayout(m_pInputLayout);

	//设置VertexShader和PixelShader
	g_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	g_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);

	//设置采样状态
	g_pDeviceContext->PSSetSamplers(0, 1, &m_pWrapLinearSampler);
	g_pDeviceContext->PSSetSamplers(1, 1, &m_pClampPointSampler);

	return true;
}


