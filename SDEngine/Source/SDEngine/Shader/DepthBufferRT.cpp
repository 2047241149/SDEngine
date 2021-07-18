#include "DepthBufferRT.h"

DepthBufferRT::DepthBufferRT(int TextureWidth, int TextureHeight)
{
	mShaderResourceView = NULL;
	mDepthStencilTexture = NULL;
	mDepthStencilView = NULL;
	
	Initialize(TextureWidth, TextureHeight);
}

DepthBufferRT::~DepthBufferRT()
{
	ShutDown();
}

bool DepthBufferRT::Initialize(int TextureWidth, int TextureHeight)
{

	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = TextureWidth;
	depthBufferDesc.Height = TextureHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;  //ע����Ȼ���(����)�İ󶨱�־
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;
	HR(g_pDevice->CreateTexture2D(&depthBufferDesc, NULL, &mDepthStencilTexture));
	
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	HR(g_pDevice->CreateDepthStencilView(mDepthStencilTexture, &depthStencilViewDesc, &mDepthStencilView));

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = depthBufferDesc.MipLevels;
	HR(g_pDevice->CreateShaderResourceView(mDepthStencilTexture, &shaderResourceViewDesc, &mShaderResourceView));
    
	mViewPort.Width = (float)TextureWidth;
	mViewPort.Height = (float)TextureHeight;
	mViewPort.MinDepth = 0.0f;
	mViewPort.MaxDepth = 1.0f;
	mViewPort.TopLeftX = 0.0f;
	mViewPort.TopLeftY = 0.0f;

	return true;
}


void DepthBufferRT::ShutDown()
{
	ReleaseCOM(mDepthStencilTexture);
	ReleaseCOM(mDepthStencilView);
	ReleaseCOM(mShaderResourceView);
}

void DepthBufferRT::SetRenderTarget()
{
	this->ClearDepth();
	ID3D11RenderTargetView* renderTarget[1] = { nullptr };
	g_pDeviceContext->OMSetRenderTargets(1,renderTarget, mDepthStencilView);
	g_pDeviceContext->RSSetViewports(1, &mViewPort);
}

void DepthBufferRT::ClearDepth()
{
	g_pDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

ID3D11ShaderResourceView* DepthBufferRT::GetShaderResourceView()
{
	return mShaderResourceView;
}