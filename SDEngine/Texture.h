#pragma once
#ifndef _TEXTURE_CLASS_H
#define _TEXTURE_CLASS_H

#include"Macro.h"
#include<d3d11_1.h>
#include "DXUT.h"
#include "SDKmisc.h"
#include "DirectxCore.h"
using namespace DirectX;

class Texture
{
private:
	ID3D11ShaderResourceView* m_pSRV;

private:
	bool Init(WCHAR* TextureFilename);

public:
	Texture(WCHAR* TextureFilename);
	Texture(const Texture& other);
	~Texture();

public:
	void ShutDown();
	ID3D11ShaderResourceView* GetTexture();

public:
	static ID3D11ShaderResourceView* LoadTexture(WCHAR* TextureFilename);

};
#endif // !_TEX_CLASS_H
