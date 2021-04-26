#pragma once
#ifndef _TEXTURE_CLASS_H
#define _TEXTURE_CLASS_H

#include "../Common/Macro.h"
#include "../Common/DirectxCore.h"
#include "../LoadTexture/Resource.h"
//#include "DXUT.h"
//#include "SDKmisc.h"
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
};
#endif // !_TEX_CLASS_H
