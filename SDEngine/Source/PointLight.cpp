#include "PointLight.h"

const float SPHERE_MESH_RADIUS = 1.0f;
const float EDGE_LIGHT_INTENSITY = 0.005f;
const float POINT_LIGHT_ATEEEN_CONSTANT = 1.0f;
const float DEFAULT_POINT_RADIUS = 15.0f;

PointLight::PointLight():Light(),
	m_fRadius(DEFAULT_POINT_RADIUS),
	m_lightAttenuation(XMFLOAT4(POINT_LIGHT_ATEEEN_CONSTANT, POINT_LIGHT_ATEEEN_CONSTANT, 0.0f,0.0f))
{
	CalculateQuadratic();
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
	CalculateQuadratic();
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

void PointLight::CalculateQuadratic()
{
	float fMaxLightIntensity = 
		FMath::Max(FMath::Max(m_LightColor.x, m_LightColor.y), m_LightColor.z) * m_fLightIntensity;

	float num1 = (fMaxLightIntensity / EDGE_LIGHT_INTENSITY) - m_lightAttenuation.x - m_lightAttenuation.y * m_fRadius;
	m_lightAttenuation.z = num1 / (m_fRadius * m_fRadius);
	m_lightAttenuation.w = sqrt(m_lightAttenuation.z * m_fRadius);
}

void PointLight::SetLightIntensity(float fLightIntensity)
{
	Light::SetLightIntensity(fLightIntensity);
	CalculateQuadratic();
}

void PointLight::SetLightColor(XMFLOAT3 color)
{
	Light::SetLightColor(color);
	CalculateQuadratic();
}