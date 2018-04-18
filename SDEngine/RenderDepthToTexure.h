#pragma once
#ifndef _RENDER_DEPTH_TO_TEXTURE_H
#define _RENDER_DEPTH_TO_TEXTURE_H

#include<Windows.h>
#include<d3d11_1.h>
#include"Macro.h"

class RenderDepthToTexture
{
private:
	ID3D11ShaderResourceView* mShaderResourceView;  //Shader资源视图
	ID3D11Texture2D* mDepthStencilTexture;
	ID3D11DepthStencilView* mDepthStencilView; 
	D3D11_VIEWPORT mViewPort; //视口


public:
	RenderDepthToTexture();
	RenderDepthToTexture(const RenderDepthToTexture&other);
	~RenderDepthToTexture();
	bool Initialize(ID3D11Device* d3dDevice,int TextureSize);
	void ShutDown();

	void SetRenderTarget(ID3D11DeviceContext* deviceContext);
	void ClearDepth(ID3D11DeviceContext* deviceContext);
	ID3D11ShaderResourceView* GetShaderResourceView();

};
#endif // !_RENDER_3D_MODEL_TO_TEXTURE_H
