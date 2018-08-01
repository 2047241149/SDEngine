#include "DiffuseNormalShader.h"


DiffuseNormalShader::DiffuseNormalShader(WCHAR* vsFilenPath, WCHAR* psFilenPath):Shader(vsFilenPath, psFilenPath)
{
	CreateBuffer();
}


DiffuseNormalShader::DiffuseNormalShader(const DiffuseNormalShader& other) :Shader(other)
{

}


DiffuseNormalShader::~DiffuseNormalShader()
{
	ShutDown();
}

void DiffuseNormalShader::ShutDown()
{
	ReleaseCOM(mCBLightBuffer)
}

void DiffuseNormalShader::CreateBuffer()
{
	mCBLightBuffer = NULL;

	D3D11_BUFFER_DESC lightBufferDesc;
	ZeroMemory(&lightBufferDesc, sizeof(lightBufferDesc));
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(CBLight);   //结构体大小,必须为16字节倍数
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	g_pDevice->CreateBuffer(&lightBufferDesc, NULL, &mCBLightBuffer);
}

bool DiffuseNormalShader::SetShaderCBExtern(CXMMATRIX worldMatrix, ID3D11ShaderResourceView* diffuseSRV, ID3D11ShaderResourceView* normalSRV)
{
	Shader::SetShaderCB(worldMatrix);

	g_pDeviceContext->PSSetShaderResources(0, 1, &diffuseSRV);
	g_pDeviceContext->PSSetShaderResources(1, 1, &normalSRV);
	return true;
}


bool DiffuseNormalShader::SetShaderParamsExtern(CXMMATRIX worldMatrix,
	ID3D11ShaderResourceView* diffuseSRV, ID3D11ShaderResourceView* normalSRV)
{

	bool result;

	//设置Shader常量缓存和纹理资源
	result = SetShaderCBExtern(worldMatrix, diffuseSRV,normalSRV);
	if (!result)
		return false;

	//设置VertexShader PixelShader InputLayout SamplerState
	SetShaderState();

	return true;
}