#pragma once
#ifndef _RENDER_TEXTURE_H
#define _RENDER_TEXTURE_H

#include<Windows.h>
#include<d3d11_1.h>
#include"D3DClass.h"
#include"Macro.h"

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
	RenderTexture(int nTextureWidth, int nTexureHeight);
	RenderTexture(const RenderTexture&other);
	~RenderTexture();


public:
	bool Init(int nTextureWidth, int nTexureHeight);
	void ShutDown();

	void SetRenderTarget();
	void ClearDepthBuffer();
	void ClearRenderTarget(float fRed, float fGreen,float fBlue, float fAlpha);

	ID3D11ShaderResourceView* GetSRV();

};
#endif 
