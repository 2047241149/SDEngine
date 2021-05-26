#include "PointLight.h"

const float SPHERE_MESH_RADIUS = 1.0f;
const float EDGE_LIGHT_INTENSITY = 0.005f;
const float POINT_LIGHT_ATEEEN_CONSTANT = 1.0f;
const float DEFAULT_POINT_RADIUS = 15.0f;

PointLight::PointLight() :Light(),
	m_fRadius(DEFAULT_POINT_RADIUS),
	m_lightAttenuation(XMFLOAT4(POINT_LIGHT_ATEEEN_CONSTANT, POINT_LIGHT_ATEEEN_CONSTANT, 1.8f, 1.0f))
{
	CalculateRadius();
}

PointLight::PointLight(const PointLight& other) :
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

XMFLOAT4 PointLight::GetLightAttenuation()
{
	return m_lightAttenuation;
}


XMMATRIX PointLight::GetWorldMatrix()
{
	float fScale = m_fRadius / SPHERE_MESH_RADIUS;
	XMMATRIX worldMatrix = XMMatrixScaling(fScale, fScale, fScale) *
		XMMatrixTranslation(m_LightPosition.x, m_LightPosition.y, m_LightPosition.z);

	return worldMatrix;
}

void PointLight::CalculateRadius()
{
	float fMaxLightIntensity =
		FMath::Max(FMath::Max(m_LightColor.x, m_LightColor.y), m_LightColor.z) * m_fLightIntensity;

	float innerNum = m_lightAttenuation.y * m_lightAttenuation.y - 4 * m_lightAttenuation.z * (m_lightAttenuation.x - fMaxLightIntensity * (256.0));
	m_fRadius = (-m_lightAttenuation.y + std::sqrt(innerNum)) / (2.0 * m_lightAttenuation.z);
}

void PointLight::SetLightIntensity(float fLightIntensity)
{
	Light::SetLightIntensity(fLightIntensity);
	CalculateRadius();
}

void PointLight::SetLightColor(XMFLOAT3 color)
{
	Light::SetLightColor(color);
	CalculateRadius();
}