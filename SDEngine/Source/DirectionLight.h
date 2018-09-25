#pragma once
#ifndef _DIRECTION_LIGHT_H
#define _DIRECTION_LIGHT_H
#include "Light.h"
#include<vector>
using std::vector;

class Camera;

class DirectionLight : public Light
{
public:
	DirectionLight();
	DirectionLight(const DirectionLight&other);
	~DirectionLight();

public:
	void SetLightDiretion(XMFLOAT3 lightDir);
	XMFLOAT3 GetLightDirection();
	
	void SetAmbientLight(XMFLOAT3  ambientLight);
	XMFLOAT3 GetAmbientLight();
	XMMATRIX GetViewMatrix();
private:
	XMFLOAT3 m_DirLightDiretion;

	XMFLOAT3 m_AmbientLight;
};

#endif