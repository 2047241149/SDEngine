#pragma once
#ifndef _TEXTURE_SAMPLER_MANAGER_H
#define _TEXTURE_SAMPLER_MANAGER_H

#include "CoreMinimal.h"

enum TextureSampler
{
	PointClamp,
	BilinearFliterClamp,
	TrilinearFliterClamp,
	PointWrap,
	BilinearFliterWrap,
	TrilinearFliterWrap,
	Anisotropic,
};

class TextureSamplerManager
{
public:
	TextureSamplerManager();
	~TextureSamplerManager();

public:
	static shared_ptr<TextureSamplerManager> Get();
	bool Init();
	ID3D11SamplerState* GetTextureSampler(TextureSampler textureSampler);

private:
	void Release();

private:
	static shared_ptr<TextureSamplerManager> single;
	map<TextureSampler, ID3D11SamplerState*> mapTextureSampler;
};


#define GTextureSamplerManager TextureSamplerManager::Get()
#define GTextureSamplerBilinearWrap	GTextureSamplerManager->GetTextureSampler(TextureSampler::BilinearFliterWrap)
#define GTextureSamplerBilinearClamp GTextureSamplerManager->GetTextureSampler(TextureSampler::BilinearFliterClamp)
#define GTextureSamplerPointWrap GTextureSamplerManager->GetTextureSampler(TextureSampler::PointWrap)
#define GTextureSamplerPointClamp GTextureSamplerManager->GetTextureSampler(TextureSampler::PointClamp)
#define GTrilinearFliterClamp GTextureSamplerManager->GetTextureSampler(TextureSampler::TrilinearFliterClamp)
#endif // !_TEXTURE_SAMPLER_MANAGER_H
