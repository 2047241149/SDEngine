#include"Light.h"

Light::Light():
	m_bCastShadow(false),
	m_LightColor(XMFLOAT3(1.0f,1.0f,1.0f)),
	m_LightPosition(XMFLOAT3(0.0f,0.0f,0.0f)),
	m_fLightIntensity(1.0f)
{

}

Light::Light(const Light&other)
{

}

Light::~Light()
{

}

void Light::SetLightColor(XMFLOAT3 color)
{
	m_LightColor = color;
}

void Light::SetLightPostion(XMFLOAT3 pos)
{
	m_LightPosition = pos;
}

XMFLOAT3 Light::GetLightColor()
{
	return XMFLOAT3(m_LightColor.x,
		m_LightColor.y, m_LightColor.z);
}


XMFLOAT3 Light::GetPosition()
{
	return m_LightPosition;
}


void Light::SetLightIntensity(float fLightIntensity)
{
	m_fLightIntensity = fLightIntensity;
}

float Light::GetLightIntensity()
{
	return m_fLightIntensity;
}