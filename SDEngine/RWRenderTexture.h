#pragma once
#ifndef _RW_RENDER_TEXTURE_H
#define _RW_RENDER_TEXTURE_H
#include "D3DClass.h"

const int STANDARD_RES_WIDTH = 1024;
const int STANDARD_RES_HEIGHT = 768;


/*
*一般用于ComputeShader的高斯模糊等后处理，因此不存在renderTarget
*通过UAV写入数据到Texture2D,Texture2D通过SRV可作为PixelShader,ComputeShader的SRV使用
*/
class RWRenderTexture
{
public:
	RWRenderTexture(int nTextureWidth = STANDARD_RES_WIDTH, int nTextureHeight = STANDARD_RES_HEIGHT);
	RWRenderTexture(const RWRenderTexture& other);
	~RWRenderTexture();
	void ShutDown();
	ID3D11ShaderResourceView* GetSRV();
	ID3D11UnorderedAccessView* GetUAV();

private:
	void Init(int nTextureWidth = STANDARD_RES_WIDTH, int nTextureHeight = STANDARD_RES_HEIGHT);


private:
	ID3D11Texture2D* m_pColorTexture;
	ID3D11ShaderResourceView* m_pSRV;
	ID3D11UnorderedAccessView* m_pUAV;
	D3D11_VIEWPORT m_RTViewport;

public:
	int m_nTextureWidth;
	int m_nTextureHeight;
};
#endif