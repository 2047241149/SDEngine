#include"DiffuseNormalSpecShader.h"

DiffuseNormalSpecShader::DiffuseNormalSpecShader(WCHAR* vsFilenPath, WCHAR* psFilenPath):
	Shader_3D(vsFilenPath, psFilenPath),
	mCBLightBuffer(nullptr),
	mCBCamera(nullptr)
{
	CreateBuffer();
}


DiffuseNormalSpecShader::DiffuseNormalSpecShader(const DiffuseNormalSpecShader& other) :
	Shader_3D(other)
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

	D3D11_BUFFER_DESC cbufferDesc;
	ZeroMemory(&cbufferDesc, sizeof(cbufferDesc));
	cbufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbufferDesc.ByteWidth = sizeof(CBLight);   //结构体大小,必须为16字节倍数
	cbufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	g_pDevice->CreateBuffer(&cbufferDesc, NULL, &mCBLightBuffer);

	cbufferDesc.ByteWidth = sizeof(CBCamera);   //结构体大小,必须为16字节倍数
	g_pDevice->CreateBuffer(&cbufferDesc, NULL, &mCBCamera);
}

bool DiffuseNormalSpecShader::SetShaderCBExtern(CXMMATRIX worldMatrix,
	ID3D11ShaderResourceView* diffuseSRV, ID3D11ShaderResourceView* normalSRV, ID3D11ShaderResourceView* specSRV)
{
	Shader_3D::SetShaderCB(worldMatrix);

	g_pDeviceContext->PSSetShaderResources(0, 1, &diffuseSRV);
	g_pDeviceContext->PSSetShaderResources(1, 1, &normalSRV);
	g_pDeviceContext->PSSetShaderResources(2, 1, &specSRV);


	return true;
}


bool DiffuseNormalSpecShader::SetShaderParamsExtern(CXMMATRIX worldMatrix,
	ID3D11ShaderResourceView* diffuseSRV, ID3D11ShaderResourceView* normalSRV, ID3D11ShaderResourceView* specSRV)
{

	bool result;

	//设置Shader常量缓存和纹理资源
	result = SetShaderCBExtern(worldMatrix, diffuseSRV,normalSRV,specSRV);
	if (!result)
		return false;

	//设置VertexShader PixelShader InputLayout SamplerState
	SetShaderState();

	return true;
}