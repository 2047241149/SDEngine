#pragma once
#ifndef _RENDER_TEXTURE_H
#define _RENDER_TEXTURE_H

#include "CoreMini.h"
#include "UtilityEnum.h"

class RenderTexture
{
private:

	ID3D11Texture2D* m_pBackTexture2D;
	ID3D11RenderTargetView* m_pRTV;
	ID3D11ShaderResourceView* m_pSRV;

	//DepthBuffer
	ID3D11Texture2D* m_pDepthTexture2D;
	ID3D11DepthStencilView* m_pDSV;
	D3D11_VIEWPORT m_ViewPort;

public:
	int m_nTextureWidth, m_nTextureHeight;

public:
	RenderTexture(int nTextureWidth, int nTexureHeight, TextureFormat eTextureFormat = TextureFormat::R32G32B32A32);
	RenderTexture(const RenderTexture&other);
	~RenderTexture();


public:
	bool Init(int nTextureWidth, int nTexureHeight, TextureFormat eTextureFormat = TextureFormat::R32G32B32A32);
	void ShutDown();

	void SetRenderTarget(float fRed = 0.0f, float fGreen = 0.0f, float fBlue = 0.0f, float fAlpha = 1.0f);
	void ClearDepthBuffer();
	void ClearRenderTarget(float fRed, float fGreen,float fBlue, float fAlpha);

	ID3D11ShaderResourceView* GetSRV();
	ID3D11RenderTargetView* GetRenderTargetView();

};
#endif 
