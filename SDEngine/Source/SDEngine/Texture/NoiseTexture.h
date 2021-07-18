#pragma once
#ifndef NOISE_TEXTURE_H
#define NOISE_TEXTURE_H
#include "CoreMinimal.h"

class NoiseTexture
{
public:
	NoiseTexture(int textureWidth, int textureHeight);
	~NoiseTexture();

private:
	ID3D11ShaderResourceView* textureSRV;

public:
	bool Init(int textureWidth, int textureHeight);
	ID3D11ShaderResourceView* GetSRV();
};

#endif