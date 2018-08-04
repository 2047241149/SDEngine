#include "PointLight.h"


PointLight::PointLight():
	Light(),
	m_fRadius(5.0f)
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