#pragma once
#ifndef _POINT_LIGHT_H
#define  _POINT_LIGHT_H
#include "Light.h"
#include "../Common/MathTool.h"

struct PointLightParams
{
	XMFLOAT3 lightPos;
	XMFLOAT3 lightColor;
	float radius;
	XMFLOAT4 attenuation;
};

class PointLight : public Light
{
public:
	PointLight();
	PointLight(const PointLight& other);
	~PointLight();

public:
	float GetRadius();
	void SetRadius(float fRadius);
	XMFLOAT4 GetLightAttenuation();
	XMMATRIX GetWorldMatrix();
	virtual void SetLightIntensity(float fLightIntensity) override;
	virtual void SetLightColor(XMFLOAT3 color) override;

private:
	void CalculateRadius();

private:
	float m_fRadius;

	//ƽ��˥��ϵ���ø������������¼��㣬��֤LightVolume�ı�Ե���ȼ��㵽0.001(δgamma����ǰ)
	XMFLOAT4 m_lightAttenuation;
};
#endif 
