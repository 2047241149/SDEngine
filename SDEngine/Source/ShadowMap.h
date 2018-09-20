#pragma once
#ifndef _SHADOW_MAP_H
#define _SHADOW_MAP_H

#include "CoreMini.h"

class ShadowMap
{
private:

	//DepthBuffer
	ID3D11Texture2D* m_pDepthTexture2D;
	ID3D11DepthStencilView* m_pDSV;
	ID3D11ShaderResourceView* m_pShadowMap;
	D3D11_VIEWPORT m_ViewPort;

public:
	int m_nTextureWidth, m_nTextureHeight;

public:
	ShadowMap(int nTextureWidth, int nTexureHeight);
	ShadowMap(const ShadowMap&other);
	~ShadowMap();

public:
	bool Init(int nTextureWidth, int nTexureHeight);
	void ShutDown();

	void SetRenderTarget();
	void ClearDepthBuffer();

	ID3D11ShaderResourceView* GetShadowMap();
	ID3D11DepthStencilView* GetDSV();

};
#endif // !_SHADOW_MAP_H
