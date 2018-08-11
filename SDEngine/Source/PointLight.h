#pragma once
#ifndef _POINT_LIGHT_H
#define  _POINT_LIGHT_H
#include "Light.h"

class PointLight : public Light
{
public:
	PointLight();
	PointLight(const PointLight& other);
	~PointLight();

public:
	float GetRadius();
	void SetRadius(float fRadius);
	XMFLOAT3 GetLightAttenuation();
	void SetLightAttenuation(XMFLOAT3 lightAttenuation);

private:
	float m_fRadius;
	XMFLOAT3 m_lightAttenuation;
};
#endif 
