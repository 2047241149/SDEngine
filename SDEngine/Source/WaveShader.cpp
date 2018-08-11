#include "WaveShader.h"

WaveShader::WaveShader(WCHAR* vsFilenPath, WCHAR* psFilenPath):
	Shader_3D(vsFilenPath, psFilenPath),
	mCBEveryFrameBuffer(nullptr),
	m_pCBDirLight(nullptr)
{
	CreateBuffer();
}


WaveShader::WaveShader(const WaveShader& other):
	Shader_3D(other)
{

}


WaveShader::~WaveShader()
{
	ShutDown();
}


void WaveShader::CreateBuffer()
{
	//CBEveryFrame
	D3D11_BUFFER_DESC cbufferDesc;
	ZeroMemory(&cbufferDesc, sizeof(cbufferDesc));
	cbufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbufferDesc.ByteWidth = sizeof(CBEveryFrame);   //结构体大小,必须为16字节倍数
	cbufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	g_pDevice->CreateBuffer(&cbufferDesc, NULL, &mCBEveryFrameBuffer);


	D3D11_BUFFER_DESC cbLightBufferDesc;
	ZeroMemory(&cbLightBufferDesc, sizeof(cbLightBufferDesc));
	cbLightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbLightBufferDesc.ByteWidth = sizeof(CBDirectionLight);   
	cbLightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbLightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	g_pDevice->CreateBuffer(&cbLightBufferDesc, NULL, &m_pCBDirLight);
}



void WaveShader::ShutDown()
{
	ReleaseCOM(mCBEveryFrameBuffer);
	ReleaseCOM(m_pCBDirLight);
}

bool WaveShader::SetShaderCBExtern(CXMMATRIX worldMatrix, FXMVECTOR surfaceColor, ID3D11ShaderResourceView* diffuse, ID3D11ShaderResourceView* normal)
{
	Shader_3D::SetShaderCB(worldMatrix);

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	HR(g_pDeviceContext->Map(mCBEveryFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource));
	auto pCBEverFrame = reinterpret_cast<CBEveryFrame*>(mappedSubresource.pData);
	XMStoreFloat4(&pCBEverFrame->surfaceColor, surfaceColor);
	g_pDeviceContext->Unmap(mCBEveryFrameBuffer, 0);

	D3D11_BUFFER_DESC dirlightBufferDesc;
	ZeroMemory(&dirlightBufferDesc, sizeof(dirlightBufferDesc));
	dirlightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	dirlightBufferDesc.ByteWidth = sizeof(CBDirectionLight);
	dirlightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	dirlightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	g_pDevice->CreateBuffer(&dirlightBufferDesc, NULL, &m_pCBDirLight);

	g_pDeviceContext->PSSetConstantBuffers(1, 1, &mCBEveryFrameBuffer);
	g_pDeviceContext->PSSetConstantBuffers(2, 1, &m_pCBDirLight);
	g_pDeviceContext->PSSetShaderResources(0, 1, &diffuse);
	g_pDeviceContext->PSSetShaderResources(1, 1, &normal);

	return true;
}
bool WaveShader::SetShaderParamsExtern(CXMMATRIX worldMatrix, FXMVECTOR surfaceColor,
	ID3D11ShaderResourceView* diffuse, ID3D11ShaderResourceView* normal)
{
	bool result;

	//设置Shader常量缓存和纹理资源
	result = SetShaderCBExtern(worldMatrix, surfaceColor, diffuse,normal);
	if (!result)
		return false;

	//设置VertexShader PixelShader InputLayout SamplerState
	SetShaderState();
	return true;
}