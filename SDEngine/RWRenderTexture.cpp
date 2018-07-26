#include "RWRenderTexture.h"

RWRenderTexture::RWRenderTexture(int nTextureWidth, int nTextureHeight):
	m_nTextureWidth(nTextureWidth),
	m_nTextureHeight(nTextureHeight)
{
	Init(nTextureWidth, nTextureHeight);
}

RWRenderTexture::RWRenderTexture(const RWRenderTexture& other)
{

}


RWRenderTexture::~RWRenderTexture()
{
	ShutDown();
}


void RWRenderTexture::ShutDown()
{
	ReleaseCOM(m_pColorTexture);
	ReleaseCOM(m_pSRV);
	ReleaseCOM(m_pUAV);
}


void RWRenderTexture::Init(int nTextureWidth, int nTextureHeight)
{
	//创建BackBufferTexture
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = nTextureWidth;
	textureDesc.Height = nTextureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;  //纹理像素为12个字节
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	g_pDevice->CreateTexture2D(&textureDesc, NULL, &m_pColorTexture);


	//创建ShaderRerouceView
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	g_pDevice->CreateShaderResourceView(m_pColorTexture, &shaderResourceViewDesc, &m_pSRV);

	//创建UnorderedAccessView
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = textureDesc.Format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;
	g_pDevice->CreateUnorderedAccessView(m_pColorTexture, &uavDesc, &m_pUAV);

	//设置渲染的视口
	m_RTViewport.Width = static_cast<float>(nTextureWidth);
	m_RTViewport.Height = static_cast<float>(nTextureHeight);
	m_RTViewport.MinDepth = 0.0f;
	m_RTViewport.MaxDepth = 1.0f;
	m_RTViewport.TopLeftX = 0.0f;
	m_RTViewport.TopLeftY = 0.0f;
}


ID3D11ShaderResourceView* RWRenderTexture::GetSRV()
{
	return m_pSRV;
}


ID3D11UnorderedAccessView* RWRenderTexture::GetUAV()
{
	return m_pUAV;
}