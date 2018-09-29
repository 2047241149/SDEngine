#pragma once
#ifndef _CASCADE_SHADOW_MAP_H
#define _CASCADE_SHADOW_MAP_H

#include "CoreMini.h"

class CascadeShadowMap
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
	CascadeShadowMap(int nTextureWidth, int nTexureHeight, int nCascadeNum);
	CascadeShadowMap(const CascadeShadowMap&other);
	~CascadeShadowMap();

public:
	bool Init(int nTextureWidth, int nTexureHeight, int nCascadeNum);
	void ShutDown();

	void SetRenderTarget(int nCascadeIndex);
	void ClearDepthBuffer();

	ID3D11ShaderResourceView* GetShadowMap();
	ID3D11DepthStencilView* GetDSV();

};
#endif // !_SHADOW_MAP_H
