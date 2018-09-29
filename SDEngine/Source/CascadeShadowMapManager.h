#pragma once
#ifndef _CASCADE_SHADOW_MAP_MANAGER_H
#define _CASCADE_SHADOW_MAP_MANAGER_H

#include "CoreMini.h"
#include "GraphicsConfig.h"
class CascadeShadowMap;

class CascadedShadowsManager
{
public:
	CascadedShadowsManager(int nShadowMapResolution);
	CascadedShadowsManager(const CascadedShadowsManager& other);
	~CascadedShadowsManager();
	
public:
	void Update();
	void InitShadowMap(int nShadowMapResolution);
	void SetRenderTarget(int nCascadeIndex);
	void ClearDepthBuffer();
	ID3D11ShaderResourceView* GetShadowMapSRV();

public:
	XMMATRIX mArrayLightOrthoMatrix[CASCADE_SHADOW_MAP_NUM];
	shared_ptr<CascadeShadowMap> mCascadeShadowMap;
	float mfCameraZ[3];

};

#endif // !_CASCADE_SHADOW_MAP_H
