#pragma once
#ifndef _TEXTURE_H
#define _TEXTURE_H

#include "SDEngine/Common/Macro.h"
#include "SDEngine/Common/DirectxCore.h"
#include "SDEngine/LoadTexture/Resource.h"
using namespace DirectX;

class Texture
{
private:
	ID3D11ShaderResourceView* m_pSRV;
	string file;

private:
	bool Init(const string& textureFile);

public:
	Texture(const string& textureFile);
	~Texture();

public:
	void ShutDown();
	ID3D11ShaderResourceView* GetTexture();
	string GetFile() { return file; }
};
#endif
