#pragma once
#ifndef _DIRECTION_LIGHT_H
#define _DIRECTION_LIGHT_H
#include "Light.h"
#include<vector>
using std::vector;

class DirectionLight : public Light
{
public:
	DirectionLight();
	DirectionLight(const DirectionLight&other);
	~DirectionLight();


public:
	void SetLookAtPosition(XMFLOAT3 lookAtPosition);
	XMFLOAT3 GetLightDirection();
	
	void SetAmbientLight(XMFLOAT3  ambientLight);
	XMFLOAT3 GetAmbientLight();
private:
	XMFLOAT3 m_LookAtPosition;
	XMFLOAT3 m_AmbientLight;
};

#endif