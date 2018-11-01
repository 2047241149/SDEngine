#include "2DShader.h"

Shader_2D::Shader_2D(WCHAR* vsFilenPath, WCHAR* psFilenPath) :
	m_pVertexShader(nullptr),
	m_pPixelShader(nullptr),
	m_pInputLayout(nullptr),
	m_pWrapLinearSampler(nullptr),
	m_pClampPointSampler(nullptr)
{
	Init(vsFilenPath, psFilenPath);
}


Shader_2D::Shader_2D(const Shader_2D& other)
{

}


Shader_2D::~Shader_2D()
{
	ShutDown();
}


bool Shader_2D::Init(WCHAR* vsFilenPath, WCHAR* psFilenPath)
{

	bool result;
	result = InitShader(vsFilenPath, psFilenPath);
	if (!result)
		return false;

	return true;
}



bool Shader_2D::SetShaderParams(ID3D11ShaderResourceView* screenRT)
{
	bool result;

	//����Shader���������������Դ
	result = SetShaderCB(screenRT);
	if (!result)
		return false;

	//����VertexShader PixelShader InputLayout SamplerState
	SetShaderState();

	return true;
}




bool Shader_2D::InitShader(WCHAR* VSFileName, WCHAR* PSFileName)
{
	HRESULT result;
	ID3D10Blob* pErrorMessage;
	ID3D10Blob* pVertexShaderBlob;
	ID3D10Blob* pPixelShaderBlob;

	//��һ,��ʼ������
	pErrorMessage = NULL;
	pVertexShaderBlob = NULL;
	pPixelShaderBlob = NULL;
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
			Log::LogShaderCompileInfo(pErrorMessage, VSFileName);
		}
		//�����ڴ�����Ϣ,Ҳ����û���ҵ�Shader�ļ�
		else
		{
			MessageBox(NULL, L"can not find VS file", L"error", MB_OK);
		}
	}

 	HR(g_pDevice->CreateVertexShader(pVertexShaderBlob->GetBufferPointer(),
		pVertexShaderBlob->GetBufferSize(), NULL, &m_pVertexShader));


	//����,����PixelShader,������PixelShader
	result = D3DCompileFromFile(PSFileName, NULL, NULL, "PS", "ps_5_0", flag, 0,
		&pPixelShaderBlob, &pErrorMessage);

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

	HR(g_pDevice->CreatePixelShader(pPixelShaderBlob->GetBufferPointer(), pPixelShaderBlob->GetBufferSize(), NULL, &m_pPixelShader));

	//����,������벼�����ݽṹ��,�������벼��
	D3D11_INPUT_ELEMENT_DESC VertexInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	unsigned int numElements = sizeof(VertexInputLayout) / sizeof(VertexInputLayout[0]);         //��������

	HR(g_pDevice->CreateInputLayout(VertexInputLayout, numElements,
		pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), &m_pInputLayout));


	
	

	//����,�ͷ�Blob
	ReleaseCOM(pVertexShaderBlob);
	ReleaseCOM(pPixelShaderBlob);


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

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	HR(g_pDevice->CreateSamplerState(&samplerDesc, &m_pClampPointSampler));

	return true;
}



void Shader_2D::ShutDown()
{
	ReleaseCOM(m_pInputLayout);
	ReleaseCOM(m_pPixelShader);
	ReleaseCOM(m_pVertexShader);
	ReleaseCOM(m_pWrapLinearSampler);
	ReleaseCOM(m_pClampPointSampler);
}

bool Shader_2D::SetShaderCB(ID3D11ShaderResourceView* screenRT)
{
	g_pDeviceContext->PSSetShaderResources(0, 1, &screenRT);

	return true;
}

void Shader_2D::SetShaderState()
{
	//���ö������벼��
	g_pDeviceContext->IASetInputLayout(m_pInputLayout);

	//����VertexShader��PixelShader
	g_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	g_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);

	//���ò���״̬
	g_pDeviceContext->PSSetSamplers(0, 1, &m_pWrapLinearSampler);
	g_pDeviceContext->PSSetSamplers(1, 1, &m_pClampPointSampler);

}


