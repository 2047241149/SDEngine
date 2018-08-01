#include"Light.h"

Light::Light():
	m_bCastShadow(false),
	m_LightColor(XMFLOAT4(1.0f,1.0f,1.0f,1.0f)),
	m_Position(XMFLOAT3(0.0f,0.0f,0.0f))
{


}

Light::Light(const Light&other)
{

}

Light::~Light()
{

}

void Light::SetLightColor(XMFLOAT4 color)
{
	m_LightColor = color;
}


void Light::SetLightPostion(XMFLOAT3 pos)
{
	m_Position = pos;
}

XMFLOAT4 Light::GetLightColor()
{
	return m_LightColor;
}


XMFLOAT3 Light::GetPosition()
{
	return m_Position;
}
