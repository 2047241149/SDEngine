#include "PointLight.h"

const float SPHERE_MESH_RADIUS = 2.0f;

PointLight::PointLight():
	Light(),
	m_fRadius(15.0f),
	m_lightAttenuation(XMFLOAT3(1.0f, 0.7f, 1.8f))
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

XMMATRIX PointLight::GetWorldMatrix()
{
	float fScale = m_fRadius / SPHERE_MESH_RADIUS;
	XMMATRIX worldMatrix = XMMatrixScaling(fScale, fScale, fScale) * 
		XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);

	return worldMatrix;
}