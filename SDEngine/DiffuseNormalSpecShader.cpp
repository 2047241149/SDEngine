#include"DiffuseNormalSpecShader.h"

DiffuseNormalSpecShader::DiffuseNormalSpecShader(WCHAR* vsFilenPath, WCHAR* psFilenPath):Shader(vsFilenPath, psFilenPath)
{
	CreateBuffer();
}


DiffuseNormalSpecShader::DiffuseNormalSpecShader(const DiffuseNormalSpecShader& other) :Shader(other)
{

}


DiffuseNormalSpecShader::~DiffuseNormalSpecShader()
{
	ShutDown();
}

void DiffuseNormalSpecShader::ShutDown()
{
	ReleaseCOM(mCBLightBuffer);
	ReleaseCOM(mCBCamera);
}

void DiffuseNormalSpecShader::CreateBuffer()
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

bool DiffuseNormalSpecShader::SetShaderCBExtern(CXMMATRIX worldMatrix,
	ID3D11ShaderResourceView* diffuseSRV, ID3D11ShaderResourceView* normalSRV, ID3D11ShaderResourceView* specSRV)
{
	ID3D11DeviceContext* d3dDeviceContext = D3DClass::GetInstance()->GetDeviceContext();
	Shader::SetShaderCB(worldMatrix);

	d3dDeviceContext->PSSetShaderResources(0, 1, &diffuseSRV);
	d3dDeviceContext->PSSetShaderResources(1, 1, &normalSRV);
	d3dDeviceContext->PSSetShaderResources(2, 1, &specSRV);


	return true;
}


bool DiffuseNormalSpecShader::SetShaderParamsExtern(CXMMATRIX worldMatrix,
	ID3D11ShaderResourceView* diffuseSRV, ID3D11ShaderResourceView* normalSRV, ID3D11ShaderResourceView* specSRV)
{

	bool result;

	//����Shader���������������Դ
	result = SetShaderCBExtern(worldMatrix, diffuseSRV,normalSRV,specSRV);
	if (!result)
		return false;

	//����VertexShader PixelShader InputLayout SamplerState
	SetShaderState();

	return true;
}