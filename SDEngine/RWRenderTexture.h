#pragma once
#ifndef _RW_RENDER_TEXTURE_H
#define _RW_RENDER_TEXTURE_H
#include "D3DClass.h"

const int STANDARD_RES_WIDTH = 1024;
const int STANDARD_RES_HEIGHT = 768;


/*
*һ������ComputeShader�ĸ�˹ģ���Ⱥ�����˲�����renderTarget
*ͨ��UAVд�����ݵ�Texture2D,Texture2Dͨ��SRV����ΪPixelShader,ComputeShader��SRVʹ��
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