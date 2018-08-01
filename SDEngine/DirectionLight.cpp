
#include "DirectionLight.h"

DirectionLight::DirectionLight() :
	Light(),
	m_LookAtPosition(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_AmbientLight(XMFLOAT3(0.0f, 0.0f, 0.0f))
{

}

DirectionLight::DirectionLight(const DirectionLight&other):
	Light(other)
{

}

DirectionLight::~DirectionLight()

{

}



void DirectionLight::SetLookAtPosition(XMFLOAT3 lookAtPosition)
{
	m_LookAtPosition = lookAtPosition;
}
	

XMFLOAT3 DirectionLight::GetLightDirection()
{
	return XMFLOAT3(m_LookAtPosition.x - m_Position.x,
		  m_LookAtPosition.y - m_Position.y,  m_LookAtPosition.z - m_Position.z);
}


void DirectionLight::SetAmbientLight(XMFLOAT3  ambientLight)
{
	m_AmbientLight = ambientLight;
}



XMFLOAT3 DirectionLight::GetAmbientLight()
{
	return m_AmbientLight;
}