#include "SSRGBuffer.h"

SSRGBuffer::SSRGBuffer(int TextureWidth, int TexureHeight, float ScreenDepth, float ScreenNear)
{
	for (int i = 0; i < SSR_BUFFER_COUNT; ++i)
	{
		mRenderTargetTextureArray[i] = NULL;
		mRenderTargetViewArray[i] = NULL;
		mGBufferSRV[i] = NULL;
	}


	Initialize(TextureWidth, TexureHeight, ScreenDepth, ScreenNear);
}

SSRGBuffer::~SSRGBuffer()
{
}

bool SSRGBuffer::Initialize(int TextureWidth, int TextureHeight, float ScreenDepth, float ScreenNear)
{

	//*************************GBuffer RenderTargetView****************************************//
	//GBuffer texture
	D3D11_TEXTURE2D_DESC gBufferTextureDesc;
	ZeroMemory(&gBufferTextureDesc, sizeof(gBufferTextureDesc));

	gBufferTextureDesc.Width = TextureWidth;
	gBufferTextureDesc.Height = TextureHeight;
	gBufferTextureDesc.MipLevels = 1;
	gBufferTextureDesc.ArraySize = 1;
	gBufferTextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	gBufferTextureDesc.SampleDesc.Count = 1;
	gBufferTextureDesc.SampleDesc.Quality = 0;
	gBufferTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	gBufferTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	gBufferTextureDesc.CPUAccessFlags = 0;
	gBufferTextureDesc.MiscFlags = 0;


	for (int i = 0; i < SSR_BUFFER_COUNT; ++i)
	{
		HR(g_pDevice->CreateTexture2D(&gBufferTextureDesc, NULL, &mRenderTargetTextureArray[i]));
	}


	//GBuffer RenderTargetView
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;

	renderTargetViewDesc.Format = gBufferTextureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	for (int i = 0; i < SSR_BUFFER_COUNT; ++i)
	{
		HR(g_pDevice->CreateRenderTargetView(mRenderTargetTextureArray[i], &renderTargetViewDesc, &mRenderTargetViewArray[i]));
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC gBufferShaderResourceViewDesc;
	gBufferShaderResourceViewDesc.Format = gBufferTextureDesc.Format;
	gBufferShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	gBufferShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	gBufferShaderResourceViewDesc.Texture2D.MipLevels = 1;

	for (int i = 0; i < SSR_BUFFER_COUNT; ++i)
	{
		HR(g_pDevice->CreateShaderResourceView(mRenderTargetTextureArray[i], &gBufferShaderResourceViewDesc, 
			&mGBufferSRV[i]));
	}

	md3dViewport.Width = static_cast<float>(TextureWidth);
	md3dViewport.Height = static_cast<float>(TextureHeight);
	md3dViewport.MinDepth = 0.0f;
	md3dViewport.MaxDepth = 1.0f;
	md3dViewport.TopLeftX = 0.0f;
	md3dViewport.TopLeftY = 0.0f;
	return true;
}


void SSRGBuffer::ShutDown()
{
	for (int i = 0; i < SSR_BUFFER_COUNT; ++i)
	{
		ReleaseCOM(mGBufferSRV[i]);
		ReleaseCOM(mRenderTargetTextureArray[i]);
		ReleaseCOM(mRenderTargetViewArray[i]);
	}
}

void SSRGBuffer::SetRenderTarget(ID3D11DepthStencilView* backDSV)
{
	g_pDeviceContext->OMSetRenderTargets(SSR_BUFFER_COUNT, mRenderTargetViewArray, backDSV);
	g_pDeviceContext->RSSetViewports(1, &md3dViewport);
	ClearGBuffer();
}

void SSRGBuffer::ClearGBuffer()
{

	float value[3] = { 0,0,0 };

	//ViewPos
	g_pDeviceContext->ClearRenderTargetView(mRenderTargetViewArray[SSRBufferType::VIEW_POS], value);
	
	//ViewNormal
	value[0] = 0.0f;
	value[1] = 0.0f;
	value[2] = -1.0f;
	g_pDeviceContext->ClearRenderTargetView(mRenderTargetViewArray[SSRBufferType::VIEW_NORMAL], value);
}

void SSRGBuffer::ClearRenderTarget()
{
	ClearGBuffer();
}

ID3D11ShaderResourceView* SSRGBuffer::GetGBufferSRV(SSRBufferType gBufferType)
{
	return mGBufferSRV[gBufferType];	
}