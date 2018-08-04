#include"DiffuseSpecShader.h"

DiffuseSpecShader::DiffuseSpecShader(WCHAR* vsFilenPath, WCHAR* psFilenPath):
	Shader_3D(vsFilenPath, psFilenPath),
	mCBLightBuffer(nullptr),
	mCBCamera(nullptr)
{
	CreateBuffer();
}


DiffuseSpecShader::DiffuseSpecShader(const DiffuseSpecShader& other):
	Shader_3D(other)
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
	D3D11_BUFFER_DESC cbufferDesc;
	ZeroMemory(&cbufferDesc, sizeof(cbufferDesc));
	cbufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbufferDesc.ByteWidth = sizeof(CBLight);  
	cbufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	g_pDevice->CreateBuffer(&cbufferDesc, NULL, &mCBLightBuffer);

	cbufferDesc.ByteWidth = sizeof(CBCamera);   
	g_pDevice->CreateBuffer(&cbufferDesc, NULL, &mCBCamera);
}

bool DiffuseSpecShader::SetShaderCBExtern(CXMMATRIX worldMatrix,
	ID3D11ShaderResourceView* diffuseSRV,  ID3D11ShaderResourceView* specSRV)
{
	Shader_3D::SetShaderCB(worldMatrix);

	g_pDeviceContext->PSSetShaderResources(0, 1, &diffuseSRV);
	g_pDeviceContext->PSSetShaderResources(1, 1, &specSRV);

	return true;
}


bool DiffuseSpecShader::SetShaderParamsExtern(CXMMATRIX worldMatrix,
	ID3D11ShaderResourceView* diffuseSRV, ID3D11ShaderResourceView* specSRV)
{
	bool result;

	//设置Shader常量缓存和纹理资源
	result = SetShaderCBExtern(worldMatrix, diffuseSRV,specSRV);
	if (!result)
		return false;

	//设置VertexShader PixelShader InputLayout SamplerState
	SetShaderState();

	return true;
}