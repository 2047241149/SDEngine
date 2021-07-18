#pragma once

/*专门用于实现SSR的GBuffer*/
#ifndef _SSR_GBUFFER_H
#define _SSR_GBUFFER_H

#include "CoreMinimal.h"

enum SSRBufferType
{
	VIEW_POS,
	VIEW_NORMAL,
};

const int SSR_BUFFER_COUNT = 2;
//1- viewPos
//2- viewNormal

class SSRGBuffer
{
private:
	//G_Buffer
	ID3D11Texture2D* mRenderTargetTextureArray[SSR_BUFFER_COUNT];
	ID3D11RenderTargetView* mRenderTargetViewArray[SSR_BUFFER_COUNT];
	ID3D11ShaderResourceView* mGBufferSRV[SSR_BUFFER_COUNT];

	D3D11_VIEWPORT md3dViewport;
	int TextureWidth, TextureHeight;


public:
	SSRGBuffer(int TextureWidth, int TexureHeight, float ScreenDepth, float ScreenNear);
	~SSRGBuffer();
	bool Initialize(int TextureWidth, int TexureHeight, float ScreenDepth, float ScreenNear);
	void ShutDown();
	
	void SetRenderTarget(ID3D11DepthStencilView* backDSV);
	void ClearGBuffer();
	void ClearRenderTarget();
	ID3D11ShaderResourceView* GetGBufferSRV(SSRBufferType gBufferType);
};
#endif // !_RENDER_3D_MODEL_TO_TEXTURE_H
