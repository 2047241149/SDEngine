#pragma once
#ifndef _CASCADE_SHADOW_MAP_H
#define _CASCADE_SHADOW_MAP_H

#include "CoreMini.h"
#include "ShadowMap.h"
#include "GraphicsConfig.h"


class CascadedShadowsManager
{
public:
	CascadedShadowsManager(int nShadowMapResolution);
	CascadedShadowsManager(const CascadedShadowsManager& other);
	~CascadedShadowsManager();
	
public:
	void Update();
	void InitShadowMap(int nShadowMapResolution);
	void SetRenderTarget(int index);
	ID3D11ShaderResourceView* GetShadowMapSRV(int index);

public:
	XMMATRIX mArrayLightOrthoMatrix[CASCADE_SHADOW_MAP_NUM];
	shared_ptr<ShadowMap> mArrayShadowMap[CASCADE_SHADOW_MAP_NUM];
	float mfCameraZ[3];

};

#endif // !_CASCADE_SHADOW_MAP_H
