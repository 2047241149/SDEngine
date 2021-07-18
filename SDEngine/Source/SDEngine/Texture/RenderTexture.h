#pragma once
#ifndef _RENDER_TEXTURE_H
#define _RENDER_TEXTURE_H

#include "CoreMinimal.h"
#include "SDEngine/Common/UtilityEnum.h"

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
	int textureWidth, textureHeight;

public:
	RenderTexture(int nTextureWidth, int nTexureHeight, TextureFormat eTextureFormat = TextureFormat::R32G32B32A32);
	~RenderTexture();

public:
	bool Init(int nTextureWidth, int nTexureHeight, TextureFormat eTextureFormat = TextureFormat::R32G32B32A32);
	void ShutDown();

	void SetRenderTarget(float fRed = 0.0f, float fGreen = 0.0f, float fBlue = 0.0f, float fAlpha = 1.0f);
	void SetRenderTargetNoClear();
	void SetDepthTarget();
	void ClearDepthBuffer();
	void ClearRenderTarget(float fRed, float fGreen,float fBlue, float fAlpha);
	void Resize(int newWidth, int newHeight);
	int GetWidth() { return textureWidth; }
	int GetHeight() { return textureHeight; }

	ID3D11ShaderResourceView* GetSRV();
	ID3D11DepthStencilView* GetDSV();
	ID3D11RenderTargetView* GetRenderTargetView();
};
#endif 
