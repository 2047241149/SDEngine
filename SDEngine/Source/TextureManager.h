#pragma once
#ifndef _TEXTURE_MANAGER_H
#define _TEXTURE_MANAGER_H
#include "CoreMini.h"
#include<d3d11_1.h>
#include<string>
#include<vector>
#include<map>
class Texture;
using std::map;
using std::vector;

class TextureManager
{
public:
	TextureManager();
	TextureManager(const TextureManager& other);
	~TextureManager();

public:
	static shared_ptr<TextureManager> Get();
	bool Init();
	void Release();

private:
	static shared_ptr<TextureManager> single;

public:
	shared_ptr<Texture> whiteTexture;

private:
	map<string, ID3D11ShaderResourceView*> srvMap;

public:
	ID3D11ShaderResourceView* LoadGetSRV(string fileName);
};

#define GTextureManager TextureManager::Get()
#define GWhiteTexture GTextureManager->whiteTexture->GetTexture()
#define LoadTexture GTextureManager->LoadGetSRV
#endif
