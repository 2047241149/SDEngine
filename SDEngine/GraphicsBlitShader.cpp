#include"GraphicsBlitShader.h"

GraphcisBlitShader::GraphcisBlitShader(WCHAR* vsFilenPath, WCHAR* psFilenPath):
	md3dVertexShader(nullptr),
	md3dPixelShader(nullptr),
	md3dInputLayout(nullptr)
{
	Init(vsFilenPath, psFilenPath);
}


GraphcisBlitShader::GraphcisBlitShader(const GraphcisBlitShader& other)
{

}


GraphcisBlitShader::~GraphcisBlitShader()
{
	ShutDown();
}


bool GraphcisBlitShader::Init(WCHAR* vsFilenPath, WCHAR* psFilenPath)
{
	bool result;
	result = InitShader(vsFilenPath, psFilenPath);
	if (!result)
		return false;

	return true;
}



bool GraphcisBlitShader::SetShaderParams(ID3D11ShaderResourceView* screenRT)
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




bool GraphcisBlitShader::InitShader(WCHAR* VSFileName, WCHAR* PSFileName)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* VertexShaderBuffer;
	ID3D10Blob* PixelShaderBuffer;


	//��һ,��ʼ������
	errorMessage = NULL;
	VertexShaderBuffer = NULL;
	PixelShaderBuffer = NULL;
	DWORD flag = D3DCOMPILE_ENABLE_STRICTNESS;

	#if _DEBUG
	flag |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	#endif

	//�ڶ�,����VertexShader����,������VertexShader
	result = D3DCompileFromFile(VSFileName, NULL, NULL, "VS", "vs_5_0", flag, 0, &VertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		//���ڴ�����Ϣ
		if (errorMessage)
		{
			Log::LogShaderCompileInfo(errorMessage,VSFileName);
		}
		//�����ڴ�����Ϣ,Ҳ����û���ҵ�Shader�ļ�
		else
		{
			MessageBox(NULL, L"can not find VS file", L"error", MB_OK);
		}
	}

	HR(g_pDevice->CreateVertexShader(VertexShaderBuffer->GetBufferPointer(), VertexShaderBuffer->GetBufferSize(), NULL, &md3dVertexShader));


	//����,����PixelShader,������PixelShader
	result = D3DCompileFromFile(PSFileName, NULL, NULL, "PS", "ps_5_0", flag, 0, &PixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		//���ڴ�����Ϣ
		if (errorMessage)
		{
			Log::LogShaderCompileInfo(errorMessage, PSFileName);
		}
		//�����ڴ�����Ϣ,Ҳ����û���ҵ�Shader�ļ�
		else
		{
			MessageBox(NULL, L"can not find PS file", L"error", MB_OK);
		}
	}

	HR(g_pDevice->CreatePixelShader(PixelShaderBuffer->GetBufferPointer(), PixelShaderBuffer->GetBufferSize(), NULL, &md3dPixelShader));

	//����,������벼�����ݽṹ��,�������벼��
	D3D11_INPUT_ELEMENT_DESC VertexInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		
	};

	unsigned int numElements = sizeof(VertexInputLayout) / sizeof(VertexInputLayout[0]);    

	HR(g_pDevice->CreateInputLayout(VertexInputLayout, numElements, VertexShaderBuffer->GetBufferPointer(), VertexShaderBuffer->GetBufferSize(), &md3dInputLayout));

	//����,�ͷ�VertexShaderBuffer��PixelShaderBuffer
	VertexShaderBuffer->Release();
	VertexShaderBuffer = NULL;
	PixelShaderBuffer->Release();
	PixelShaderBuffer = NULL;

	//����,���������ݽṹ��,���Ҵ�������״̬
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;  //�������Բ�ֵ(���ַ�ʽ,�����,���Թ���,���Թ���)
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

	HR(g_pDevice->CreateSamplerState(&samplerDesc, &mSamplerLinearWrap));

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	HR(g_pDevice->CreateSamplerState(&samplerDesc, &mSamplerLinearClamp));

	return true;
}



void GraphcisBlitShader::ShutDown()
{
	ReleaseCOM(md3dInputLayout);
	ReleaseCOM(md3dPixelShader);
	ReleaseCOM(md3dVertexShader);
}

bool GraphcisBlitShader::SetShaderCB(ID3D11ShaderResourceView* screenRT)
{

	g_pDeviceContext->PSSetShaderResources(0, 1, &screenRT);

	return true;
}


bool GraphcisBlitShader::SetShaderState()
{

	//���ö������벼��
	g_pDeviceContext->IASetInputLayout(md3dInputLayout);

	//����VertexShader��PixelShader
	g_pDeviceContext->VSSetShader(md3dVertexShader, NULL, 0);
	g_pDeviceContext->PSSetShader(md3dPixelShader, NULL, 0);

	//���ò���״̬
	g_pDeviceContext->PSSetSamplers(0, 1, &mSamplerLinearWrap);
	g_pDeviceContext->PSSetSamplers(1, 1, &mSamplerLinearClamp);

	return true;
}


