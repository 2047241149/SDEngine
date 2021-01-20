#pragma once
#ifndef _Hdr_TEXTURE_CLASS_H
#define _Hdr_TEXTURE_CLASS_H

#include "../Common/Macro.h"
#include "../Common/DirectxCore.h"
#include "../LoadTexture/Resource.h"
//#include "DXUT.h"
//#include "SDKmisc.h"
using namespace DirectX;

class HdrTexture
{
private:
	ID3D11ShaderResourceView* m_pSRV;

private:
	bool Init(char*);

public:
	HdrTexture(char* TextureFilename);
	HdrTexture(const HdrTexture& other);
	~HdrTexture();

public:
	void ShutDown();
	ID3D11ShaderResourceView* GetTexture();

};
#endif // !_TEX_CLASS_H
