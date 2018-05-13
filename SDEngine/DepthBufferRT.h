#pragma once
#ifndef _RENDER_DEPTH_TO_TEXTURE_H
#define _RENDER_DEPTH_TO_TEXTURE_H

#include<Windows.h>
#include<d3d11_1.h>
#include"Macro.h"
#include "D3DClass.h"

class DepthBufferRT
{
private:
	ID3D11ShaderResourceView* mShaderResourceView;  //Shader资源视图
	ID3D11Texture2D* mDepthStencilTexture;
	ID3D11DepthStencilView* mDepthStencilView; 
	D3D11_VIEWPORT mViewPort; //视口


public:
	DepthBufferRT(int TextureWidth, int TextureHeight);
	DepthBufferRT(const DepthBufferRT&other);
	~DepthBufferRT();
	bool Initialize(int TextureWidth, int TextureHeight);
	void ShutDown();

	void SetRenderTarget();
	void ClearDepth();
	ID3D11ShaderResourceView* GetShaderResourceView();

};
#endif // !_RENDER_3D_MODEL_TO_TEXTURE_H
