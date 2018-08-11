#include"PureColorShader.h"

PureColorShader::PureColorShader(WCHAR* vsFilenPath, WCHAR* psFilenPath):
	Shader_3D(vsFilenPath, psFilenPath),
	mCBEveryFrameBuffer(nullptr),
	m_pCBDirLight(nullptr)
{
	CreateConstantBuffer();
}


PureColorShader::PureColorShader(const PureColorShader& other):
	Shader_3D(other)
{

}


PureColorShader::~PureColorShader()
{
	ShutDown();
}


void PureColorShader::CreateConstantBuffer()
{

	//CBEveryFrame
	D3D11_BUFFER_DESC cbufferDesc;
	ZeroMemory(&cbufferDesc, sizeof(cbufferDesc));
	cbufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbufferDesc.ByteWidth = sizeof(CBEveryFrame);  
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



void PureColorShader::ShutDown()
{
	ReleaseCOM(mCBEveryFrameBuffer);
	ReleaseCOM(m_pCBDirLight);
}

bool PureColorShader::SetShaderCBExtern(CXMMATRIX worldMatrix, FXMVECTOR surfaceColor)
{

	Shader_3D::SetShaderCB(worldMatrix);

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	HR(g_pDeviceContext->Map(mCBEveryFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource));
	auto pCBEverFrame = reinterpret_cast<CBEveryFrame*>(mappedSubresource.pData);
	XMStoreFloat4(&pCBEverFrame->surfaceColor, surfaceColor);
	g_pDeviceContext->Unmap(mCBEveryFrameBuffer, 0);

	D3D11_MAPPED_SUBRESOURCE mappedSSLight;
	HR(g_pDeviceContext->Map(m_pCBDirLight, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSSLight));
	auto pCBDirLght = reinterpret_cast<CBDirectionLight*>(mappedSSLight.pData);
	pCBDirLght->ambientLight = GLightManager->GetMainLight()->GetAmbientLight();
	pCBDirLght->lightColor = GLightManager->GetMainLight()->GetLightColor();
	pCBDirLght->lightDir = GLightManager->GetMainLight()->GetLightDirection();
	pCBDirLght->pad = XMFLOAT2(0.0f, 0.0f);
	g_pDeviceContext->Unmap(m_pCBDirLight, 0);

	g_pDeviceContext->PSSetConstantBuffers(1, 1, &mCBEveryFrameBuffer);
	g_pDeviceContext->PSSetConstantBuffers(2, 1, &m_pCBDirLight);


	return true;
}
bool PureColorShader::SetShaderParamsExtern(CXMMATRIX worldMatrix, FXMVECTOR surfaceColor)
{
	bool result;

	//设置Shader常量缓存和纹理资源
	result = SetShaderCBExtern(worldMatrix, surfaceColor);
	if (!result)
		return false;

	//设置VertexShader PixelShader InputLayout SamplerState
	SetShaderState();
	return true;
}