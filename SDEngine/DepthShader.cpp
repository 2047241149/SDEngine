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
	//�ÿ�ָ��
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



bool DepthShader::SetShaderParams(CXMMATRIX uiViewMatrix, CXMMATRIX uiOrhoMatrix, ID3D11ShaderResourceView* diffuseTexture)
{
	bool result;

	//����Shader���������������Դ
	result = SetShaderCB(uiViewMatrix, uiOrhoMatrix, diffuseTexture);
	if (!result)
		return false;

	//����VertexShader PixelShader InputLayout SamplerState
	SetShaderState();

	return true;
}




bool DepthShader::InitializeShader(WCHAR* VSFileName, WCHAR* PSFileName)
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
			OutputShaderErrorMessage(errorMessage, VSFileName);
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
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // 96λ��12���ֽ�
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};

	unsigned int numElements = sizeof(VertexInputLayout) / sizeof(VertexInputLayout[0]);         //��������

	HR(d3dDevice->CreateInputLayout(VertexInputLayout, numElements, VertexShaderBuffer->GetBufferPointer(), VertexShaderBuffer->GetBufferSize(), &md3dInputLayout));

	//����,�ͷ�VertexShaderBuffer��PixelShaderBuffer
	VertexShaderBuffer->Release();
	VertexShaderBuffer = NULL;
	PixelShaderBuffer->Release();
	PixelShaderBuffer = NULL;

	//����,����(�任������)�������ݽṹ��,����������������
	D3D11_BUFFER_DESC matrixBufferDesc;
	ZeroMemory(&matrixBufferDesc, sizeof(matrixBufferDesc));
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(CBMatrix);   //�ṹ���С,����Ϊ16�ֽڱ���
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HR(d3dDevice->CreateBuffer(&matrixBufferDesc, NULL, &mCBMatrixBuffer));

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

	HR(d3dDevice->CreateSamplerState(&samplerDesc, &mSamplerState));

	return true;
}



void DepthShader::ShutDown()
{
	ReleaseCOM(mCBMatrixBuffer);
	ReleaseCOM(md3dInputLayout);
	ReleaseCOM(md3dPixelShader);
	ReleaseCOM(md3dVertexShader);
}

void DepthShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, WCHAR* shaderFilename)
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


bool DepthShader::SetShaderCB(CXMMATRIX uiViewMatrix, CXMMATRIX uiOrhoMatrix, ID3D11ShaderResourceView* diffuseTexture)
{

	ID3D11DeviceContext* d3dDeviceContext = D3DClass::GetInstance()->GetDeviceContext();

	//��һ�����±任�����������ֵ
	//������ת��,�ڴ��볣������ǰ����ת��,��ΪGPU�Ծ������ݻ��Զ�����һ��ת��
	D3D11_MAPPED_SUBRESOURCE mappedSubresource1;
	HR(d3dDeviceContext->Map(mCBMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource1));
	auto pCBMatrix = reinterpret_cast<CBMatrix*>(mappedSubresource1.pData);
	XMMATRIX uiViewMa = XMMatrixTranspose(uiViewMatrix);
	XMMATRIX uiOrthoMa = XMMatrixTranspose(uiOrhoMatrix);
	pCBMatrix->mUIViewMatrix = uiViewMa;
	pCBMatrix->mUIOrthoMatrix = uiOrthoMa;
	d3dDeviceContext->Unmap(mCBMatrixBuffer, 0);


	//����,������VertexShader�ĳ��������ֵ(���Ÿ��µ�ֵ)
	d3dDeviceContext->VSSetConstantBuffers(0, 1, &mCBMatrixBuffer);

	d3dDeviceContext->PSSetShaderResources(0, 1, &diffuseTexture);

	return true;
}

bool DepthShader::SetShaderState()
{

	ID3D11DeviceContext* deviceContext = D3DClass::GetInstance()->GetDeviceContext();

	//���ö������벼��
	deviceContext->IASetInputLayout(md3dInputLayout);

	//����VertexShader��PixelShader
	deviceContext->VSSetShader(md3dVertexShader, NULL, 0);
	deviceContext->PSSetShader(md3dPixelShader, NULL, 0);

	//���ò���״̬
	deviceContext->PSSetSamplers(0, 1, &mSamplerState);  //S0ע�� ��Ӧ0

	return true;
}


