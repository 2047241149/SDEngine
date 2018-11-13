#pragma once
#ifndef NOISE_TEXTURE_H
#define NOISE_TEXTURE_H
#include "CoreMini.h"

class NoiseTexture
{
public:
	NoiseTexture(int textureWidth, int textureHeight);
	NoiseTexture(const NoiseTexture& other);
	~NoiseTexture();

private:
	ID3D11ShaderResourceView* textureSRV;

public:
	bool Init(int textureWidth, int textureHeight);
	ID3D11ShaderResourceView* GetSRV();
};

#endif