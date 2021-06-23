#pragma once
#ifndef _TEXTURE_MANAGER_H
#define _TEXTURE_MANAGER_H
#include "CoreMinimal.h"

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
	static shared_ptr<TextureManager> Get();
};

#define GTextureManager TextureManager::Get()
#define GWhiteTexture GTextureManager->whiteTexture
#define GWhiteTextureSrv GTextureManager->whiteTexture->GetTexture()
#define LoadTexture GTextureManager->LoadGetSRV
#endif
