#pragma once
#ifndef _RENDER_CUBEMAP_H
#define _RENDER_CUBEMAP_H

#include "CoreMinimal.h"
#include "SDEngine/Common/UtilityEnum.h"

class RenderCubeMap
{
private:

	ID3D11Texture2D* cubeMapTexture;
	ID3D11RenderTargetView* rtvs[6];
	ID3D11ShaderResourceView* srv;

	//DepthBuffer
	ID3D11Texture2D* depthTexture;
	ID3D11DepthStencilView* dsv;
	D3D11_VIEWPORT viewport;

public:
	int textureWidth, textureHeight;

public:
	RenderCubeMap(int nTextureWidth, int nTexureHeight);
	RenderCubeMap(const RenderCubeMap&other);
	~RenderCubeMap();


public:
	bool Init(int inTextureWidth, int inTextureHeight);
	void ShutDown();

	void SetRenderTarget(int targetViewSlot);
	void ClearDepthBuffer();
	void ClearRenderTarget(float fRed, float fGreen,float fBlue, float fAlpha);

	ID3D11ShaderResourceView* GetSRV();
	ID3D11RenderTargetView* GetRenderTargetView(int targetViewSlot);

};
#endif 
