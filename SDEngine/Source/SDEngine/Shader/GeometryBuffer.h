#pragma once
#ifndef _GEOMETRY_BUFFER_H
#define _GEOMETRY_BUFFER_H

#include "../Common/CoreMini.h"
#include<ctime>


enum GBufferType
{
	Diffuse,
	Pos,
	Normal,
	SpecularRoughMetal,
	Depth,
};



const int BUFFER_COUNT = 4;
//1- diffuse
//2- pos
//3- normal
//4- specular-rough-metal
//另外加一个depthBuffer
class GeometryBuffer
{
private:
	//G_Buffer
	ID3D11Texture2D* mRenderTargetTextureArray[BUFFER_COUNT];
	ID3D11RenderTargetView* mRenderTargetViewArray[BUFFER_COUNT];
	ID3D11ShaderResourceView* mGBufferSRV[BUFFER_COUNT];

	//DepthBuffer
	ID3D11Texture2D* mDepthStencilTexture;
	ID3D11DepthStencilView* mDepthStencilView;
	ID3D11ShaderResourceView* mDepthBufferSRV;
	D3D11_VIEWPORT md3dViewport;
	int TextureWidth, TextureHeight;

	//RandomRT
	ID3D11Texture2D* mRandomTexture;
	ID3D11ShaderResourceView* mRandomSRV;

public:
	GeometryBuffer(int TextureWidth, int TexureHeight, float ScreenDepth, float ScreenNear);
	GeometryBuffer(const GeometryBuffer&other);
	~GeometryBuffer();
	bool Initialize(int TextureWidth, int TexureHeight, float ScreenDepth, float ScreenNear);
	void ShutDown();

	void SetRenderTarget(XMFLOAT3 backColor);
	void SetDepthTarget();
	void ClearDepthBuffer();
	void ClearGBuffer(XMFLOAT3 backColor);
	void ClearRenderTarget(XMFLOAT3 backColor);
	ID3D11ShaderResourceView* GetGBufferSRV(GBufferType gBufferType);
	ID3D11ShaderResourceView* GetRandomRTSRV();
	ID3D11DepthStencilView* GetDSV();

};
#endif // !_RENDER_3D_MODEL_TO_TEXTURE_H
