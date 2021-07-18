#pragma once
#ifndef _SSAO_MANAGER_H
#define _SSAO_MANAGER_H

#include "CoreMinimal.h"
class RenderTexture;
class NoiseTexture;
class Quad;

class SSAOManager
{
public:
	SSAOManager(int screenWidth, int screenHeight);
	~SSAOManager();

public:
	void Init(int screenWidth, int screenHeight);
	void ShutDown();
	void Render(class GeometryBuffer* geometryBuffer);
	ID3D11ShaderResourceView* GetSsaoSRV();

private:
	vector<XMFLOAT3> vecSSAOSample;
	shared_ptr<RenderTexture> ssaoRT;
	shared_ptr<RenderTexture> ssaoDownSampleRT;
	shared_ptr<RenderTexture> ssaoDownSampleBlurRT;
	shared_ptr<RenderTexture> ssaoUpSampleRT;
	shared_ptr<NoiseTexture> ssaoNoiseTexture;
	shared_ptr<Quad> quad;
};
#endif // !_SSAO_MANAGER
