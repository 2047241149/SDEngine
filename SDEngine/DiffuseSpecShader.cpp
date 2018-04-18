#include"DiffuseSpecShader.h"

DiffuseSpecShader::DiffuseSpecShader(WCHAR* vsFilenPath, WCHAR* psFilenPath):Shader(vsFilenPath, psFilenPath)
{
	CreateBuffer();
}


DiffuseSpecShader::DiffuseSpecShader(const DiffuseSpecShader& other) :Shader(other)
{

}


DiffuseSpecShader::~DiffuseSpecShader()
{
	ShutDown();
}

void DiffuseSpecShader::ShutDown()
{
	ReleaseCOM(mCBLightBuffer);
	ReleaseCOM(mCBCamera);
}

void DiffuseSpecShader::CreateBuffer()
{
	mCBLightBuffer = NULL;

	ID3D11Device* d3dDevice = D3DClass::GetInstance()->GetDevice();

	D3D11_BUFFER_DESC cbufferDesc;
	ZeroMemory(&cbufferDesc, sizeof(cbufferDesc));
	cbufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbufferDesc.ByteWidth = sizeof(CBLight);   //�ṹ���С,����Ϊ16�ֽڱ���
	cbufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	d3dDevice->CreateBuffer(&cbufferDesc, NULL, &mCBLightBuffer);

	cbufferDesc.ByteWidth = sizeof(CBCamera);   //�ṹ���С,����Ϊ16�ֽڱ���
	d3dDevice->CreateBuffer(&cbufferDesc, NULL, &mCBCamera);
}

bool DiffuseSpecShader::SetShaderCBExtern(CXMMATRIX worldMatrix,
	ID3D11ShaderResourceView* diffuseSRV,  ID3D11ShaderResourceView* specSRV)
{
	ID3D11DeviceContext* d3dDeviceContext = D3DClass::GetInstance()->GetDeviceContext();
	Shader::SetShaderCB(worldMatrix);

	d3dDeviceContext->PSSetShaderResources(0, 1, &diffuseSRV);
	d3dDeviceContext->PSSetShaderResources(1, 1, &specSRV);

	return true;
}


bool DiffuseSpecShader::SetShaderParamsExtern(CXMMATRIX worldMatrix,
	ID3D11ShaderResourceView* diffuseSRV, ID3D11ShaderResourceView* specSRV)
{
	bool result;

	//����Shader���������������Դ
	result = SetShaderCBExtern(worldMatrix, diffuseSRV,specSRV);
	if (!result)
		return false;

	//����VertexShader PixelShader InputLayout SamplerState
	SetShaderState();

	return true;
}