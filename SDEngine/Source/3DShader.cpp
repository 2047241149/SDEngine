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
	//����Shader���������������Դ
	result = SetShaderCB(worldMatrix);
	if (!result)
		return false;

	//����VertexShader PixelShader InputLayout SamplerState
	SetShaderState();

	return true;
}

bool Shader_3D::InitShader(WCHAR* VSFileName, WCHAR* PSFileName)
{
	HRESULT result;
	ID3D10Blob* pErrorMessage;
	ID3D10Blob* pVertexShaderBlob;
	ID3D10Blob* pPixelShaderBob;

	//��һ,��ʼ������
	pErrorMessage = nullptr;
	pVertexShaderBlob = nullptr;
	pPixelShaderBob = nullptr;
	DWORD flag = D3DCOMPILE_ENABLE_STRICTNESS;

	#if _DEBUG
	flag |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	#endif

	//�ڶ�,����VertexShader����,������VertexShader
	result = D3DCompileFromFile(VSFileName, NULL, NULL, "VS", "vs_5_0", flag, 0, &pVertexShaderBlob, &pErrorMessage);
	if (FAILED(result))
	{
		//���ڴ�����Ϣ
		if (pErrorMessage)
		{
			Log::LogShaderCompileInfo(pErrorMessage,VSFileName);
		}
		//�����ڴ�����Ϣ,Ҳ����û���ҵ�Shader�ļ�
		else
		{
			MessageBox(NULL, L"can not find VS file", L"error", MB_OK);
		}
	}

	HR(g_pDevice->CreateVertexShader(pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), NULL, &m_pVertexShader));


	//����,����PixelShader,������PixelShader
	result = D3DCompileFromFile(PSFileName, NULL, NULL, "PS", "ps_5_0", flag, 0, &pPixelShaderBob, &pErrorMessage);
	if (FAILED(result))
	{
		//���ڴ�����Ϣ
		if (pErrorMessage)
		{
			Log::LogShaderCompileInfo(pErrorMessage, PSFileName);
		}
		//�����ڴ�����Ϣ,Ҳ����û���ҵ�Shader�ļ�
		else
		{
			MessageBox(NULL, L"can not find PS file", L"error", MB_OK);
		}
	}

	HR(g_pDevice->CreatePixelShader(pPixelShaderBob->GetBufferPointer(), pPixelShaderBob->GetBufferSize(), NULL, &m_pPixelShader));

	//����,������벼�����ݽṹ��,�������벼��
	D3D11_INPUT_ELEMENT_DESC VertexInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	//��������
	unsigned int numElements = sizeof(VertexInputLayout) / sizeof(VertexInputLayout[0]);         

	HR(g_pDevice->CreateInputLayout(VertexInputLayout, numElements, pVertexShaderBlob->GetBufferPointer(),
		pVertexShaderBlob->GetBufferSize(), &m_pInputLayout));

	ID3D11ShaderReflection* pReflection = nullptr;
	HR(D3DReflect(pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pReflection));
	D3D11_SHADER_DESC vertexShaderDesc;
	HR(pReflection->GetDesc(&vertexShaderDesc));
	
	for (int nCBIndex = 0; nCBIndex < vertexShaderDesc.ConstantBuffers; ++nCBIndex)
	{
		unsigned int register_index = 0;
		ID3D11ShaderReflectionConstantBuffer* buffer = nullptr;
		buffer = pReflection->GetConstantBufferByIndex(nCBIndex);

		D3D11_SHADER_BUFFER_DESC shaderBufferDesc;
		buffer->GetDesc(&shaderBufferDesc);
		for (int s = 0; s < shaderBufferDesc.Variables; ++s)
		{
			ID3D11ShaderReflectionVariable* pShaderRefVarible = buffer->GetVariableByIndex(s);
			D3D11_SHADER_VARIABLE_DESC shaderVarDesc;
			pShaderRefVarible->GetDesc(&shaderVarDesc);
			int a = shaderVarDesc.StartOffset;
		}
	
	
		for (int nBindResourceIndex = 0; nBindResourceIndex < vertexShaderDesc.BoundResources; ++nBindResourceIndex)
		{
			D3D11_SHADER_INPUT_BIND_DESC shaderInputBindDesc;
			pReflection->GetResourceBindingDesc(nBindResourceIndex, &shaderInputBindDesc);
			if (!strcmp(shaderInputBindDesc.Name, shaderBufferDesc.Name))
				register_index = shaderInputBindDesc.BindPoint;
		}
	}


	HR(D3DReflect(pPixelShaderBob->GetBufferPointer(), pPixelShaderBob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pReflection));
	HR(pReflection->GetDesc(&vertexShaderDesc));


	for (int nCBIndex = 0; nCBIndex < vertexShaderDesc.ConstantBuffers; ++nCBIndex)
	{
		unsigned int register_index = 0;
		ID3D11ShaderReflectionConstantBuffer* buffer = nullptr;
		buffer = pReflection->GetConstantBufferByIndex(nCBIndex);

		D3D11_SHADER_BUFFER_DESC shaderBufferDesc;
		buffer->GetDesc(&shaderBufferDesc);

		for (int nBindResourceIndex = 0; nBindResourceIndex < vertexShaderDesc.BoundResources; ++nBindResourceIndex)
		{
			D3D11_SHADER_INPUT_BIND_DESC shaderInputBindDesc;
			pReflection->GetResourceBindingDesc(nBindResourceIndex, &shaderInputBindDesc);
			if (!strcmp(shaderInputBindDesc.Name, shaderBufferDesc.Name))
				register_index = shaderInputBindDesc.BindPoint;
		}
	}



	//����,�ͷ�pVertexShaderBlob��pPixelShaderBob
	ReleaseCOM(pVertexShaderBlob);
	ReleaseCOM(pPixelShaderBob);



	//����,����(�任������)�������ݽṹ��,����������������
	D3D11_BUFFER_DESC commonBufferDesc;
	ZeroMemory(&commonBufferDesc, sizeof(commonBufferDesc));
	commonBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	commonBufferDesc.ByteWidth = sizeof(CBCommmon);  
	commonBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	commonBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	HR(g_pDevice->CreateBuffer(&commonBufferDesc, NULL, &m_pCBCommon));

	//����,���������ݽṹ��,���Ҵ�������״̬
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

	//��һ�����±任�����������ֵ
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

	//����,������VertexShader�ĳ��������ֵ(���Ÿ��µ�ֵ)
	g_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pCBCommon);
	g_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pCBCommon);

	return true;
}

bool Shader_3D::SetShaderState()
{
	//���ö������벼��
	g_pDeviceContext->IASetInputLayout(m_pInputLayout);

	//����VertexShader��PixelShader
	g_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	g_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);

	//���ò���״̬
	g_pDeviceContext->PSSetSamplers(0, 1, &m_pWrapLinearSampler);
	g_pDeviceContext->PSSetSamplers(1, 1, &m_pClampPointSampler);

	return true;
}


