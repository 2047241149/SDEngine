#include "PointLight.h"


PointLight::PointLight():
	Light(),
	m_fRadius(5.0f),
	m_lightAttenuation(XMFLOAT3(1.0f,0.0f,0.0f))
{

}

PointLight::PointLight(const PointLight& other):
	Light(other)
{

}

PointLight::~PointLight()
{

}

float PointLight::GetRadius()
{
	return m_fRadius;
}

void PointLight::SetRadius(float fRadius)
{
	m_fRadius = fRadius;
}

XMFLOAT3 PointLight::GetLightAttenuation()
{
	return m_lightAttenuation;
}

void PointLight::SetLightAttenuation(XMFLOAT3 lightAttenuation)
{
	m_lightAttenuation = lightAttenuation;
}