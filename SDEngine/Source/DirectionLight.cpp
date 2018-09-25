
#include "DirectionLight.h"
#include "Camera.h"
#include <math.h>

DirectionLight::DirectionLight() :
	Light(),
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

void DirectionLight::SetAmbientLight(XMFLOAT3  ambientLight)
{
	m_AmbientLight = ambientLight;
}

XMFLOAT3 DirectionLight::GetAmbientLight()
{
	return m_AmbientLight;
}

void DirectionLight::SetLightDiretion(XMFLOAT3 lightDir)
{
	m_DirLightDiretion = lightDir;
}


XMFLOAT3 DirectionLight::GetLightDirection()
{
	return m_DirLightDiretion;
}

XMMATRIX DirectionLight::GetViewMatrix()
{
	XMVECTOR eyePosVec = XMVectorSet(m_Position.x, m_Position.y, m_Position.z, 1.0f);
	XMVECTOR targetPosVec = XMVectorSet(m_Position.x + m_DirLightDiretion.x, m_Position.y + m_DirLightDiretion.y, m_Position.z + m_DirLightDiretion.z, 1.0f);

	XMVECTOR upVec = XMVectorSet(0.0, 1.0f, 0.0f, 0.0f);
	return XMMatrixLookAtLH(eyePosVec, targetPosVec, upVec);
}

