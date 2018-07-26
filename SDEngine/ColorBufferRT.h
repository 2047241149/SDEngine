#pragma once
#ifndef _COLOR_BUFFER_CLASS_H
#define _COLOR_BUFFER_CLASS_H

#include<Windows.h>
#include<d3d11_1.h>
#include"D3DClass.h"
#include"Macro.h"

class ColorBufferRT
{
private:
	ID3D11Texture2D* mRenderTargetTexture;
	ID3D11RenderTargetView* mRenderTargetView;
	ID3D11ShaderResourceView* mShaderResourceView;

	//DepthBuffer
	ID3D11Texture2D* mDepthStencilBuffer;
	ID3D11DepthStencilView* mDepthStencilView;
	D3D11_VIEWPORT md3dViewport;

public:
	int m_nTextureWidth, m_nTextureHeight;

public:
	ColorBufferRT(int TextureWidth, int TexureHeight, float ScreenDepth, float ScreenNear);
	ColorBufferRT(const ColorBufferRT&other);
	~ColorBufferRT();
	bool Initialize(int TextureWidth,int TexureHeight,float ScreenDepth,float ScreenNear);	
	void ShutDown();

	void SetRenderTarget();
	void ClearDepthBuffer();
	void ClearRenderTarget(float red, float green, float blue, float alpha);
	ID3D11ShaderResourceView* GetShaderResourceView();

};
#endif // !_RENDER_3D_MODEL_TO_TEXTURE_H
