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
	//�ÿ�ָ��
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
	//����Shader���������������Դ
	result = SetShaderCB(screenRT,screenBlurRT, depthRT,dofStart,dofRange,farPlane,nearPlane);
	if (!result)
		return false;

	//����VertexShader PixelShader InputLayout SamplerState
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
			OutputShaderErrorMessage(errorMessage,VSFileName);
		}
		//�����ڴ�����Ϣ,Ҳ����û���ҵ�Shader�ļ�
		else
		{
			MessageBox(NULL, L"can not find VS file", L"error", MB_OK);
		}
	}

	HR(d3dDevice->CreateVertexShader(VertexShaderBuffer->GetBufferPointer(), VertexShaderBuffer->GetBufferSize(), NULL, &md3dVertexShader));


	//����,����PixelShader,������PixelShader
	result = D3DCompileFromFile(PSFileName, NULL, NULL, "PS", "ps_5_0", flag, 0, &PixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		//���ڴ�����Ϣ
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, PSFileName);
		}
		//�����ڴ�����Ϣ,Ҳ����û���ҵ�Shader�ļ�
		else
		{
			MessageBox(NULL, L"can not find PS file", L"error", MB_OK);
		}
	}

	HR(d3dDevice->CreatePixelShader(PixelShaderBuffer->GetBufferPointer(), PixelShaderBuffer->GetBufferSize(), NULL, &md3dPixelShader));

	//����,������벼�����ݽṹ��,�������벼��
	D3D11_INPUT_ELEMENT_DESC VertexInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		
	};

	unsigned int numElements = sizeof(VertexInputLayout) / sizeof(VertexInputLayout[0]);         //��������

	HR(d3dDevice->CreateInputLayout(VertexInputLayout, numElements, VertexShaderBuffer->GetBufferPointer(), VertexShaderBuffer->GetBufferSize(), &md3dInputLayout));

	//����,�ͷ�VertexShaderBuffer��PixelShaderBuffer
	VertexShaderBuffer->Release();
	VertexShaderBuffer = NULL;
	PixelShaderBuffer->Release();
	PixelShaderBuffer = NULL;

	D3D11_BUFFER_DESC cBufferDesc;
	ZeroMemory(&cBufferDesc, sizeof(cBufferDesc));
	cBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cBufferDesc.ByteWidth = sizeof(CBDOF);   //�ṹ���С,����Ϊ16�ֽڱ���
	cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HR(d3dDevice->CreateBuffer(&cBufferDesc, NULL, &mCBDOF));

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


	// ��ȡָ�������Ϣ�ı���ָ��
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// ��ȡ������Ϣ�ı��ĳ���
	bufferSize = errorMessage->GetBufferSize();

	// ����һ��txt,����д�������Ϣ
	fout.open("shader-error.txt");

	//��txt�ļ�д�������Ϣ
	for (i = 0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// �ر��ļ�
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	//�������ѵ�С����
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

	//���ö������벼��
	deviceContext->IASetInputLayout(md3dInputLayout);

	//����VertexShader��PixelShader
	deviceContext->VSSetShader(md3dVertexShader, NULL, 0);
	deviceContext->PSSetShader(md3dPixelShader, NULL, 0);

	//���ò���״̬
	deviceContext->PSSetSamplers(0, 1, &mSamplerLinearWrap); 
	deviceContext->PSSetSamplers(1, 1, &mSamplerLinearClamp);

	return true;
}


