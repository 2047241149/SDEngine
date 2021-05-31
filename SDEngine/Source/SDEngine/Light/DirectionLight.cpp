#include "DirectionLight.h"
#include "../GameObject/Camera.h"
#include <math.h>

DirectionLight::DirectionLight() :
	Light()
{

}

DirectionLight::DirectionLight(const DirectionLight&other):
	Light(other)
{

}

DirectionLight::~DirectionLight()
{
}

void DirectionLight::SetLightDiretion(XMFLOAT3 lightDir)
{
	m_DirLightDiretion = lightDir;
}

XMFLOAT3 DirectionLight::GetLightDirection()
{
	return m_DirLightDiretion;
}

//�����λ�õ���ƽ�й�Դ��λ��
XMMATRIX DirectionLight::GetViewMatrix()
{
	float fFarPlane = GCamera->mFarPlane;
	XMFLOAT3 eyePos = m_LightPosition;
	XMVECTOR eyePosVec = XMVectorSet(eyePos.x, eyePos.y, eyePos.z, 1.0f);
	XMVECTOR normalizeLightDir = XMVector3Normalize(XMLoadFloat3(&m_DirLightDiretion));
	//eyePosVec += XMVectorMultiply(normalizeLightDir, 0.5f * fFarPlane * XMVectorSet(-1.0f, -1.0f, -1.0f, 0.0f));
	XMVECTOR targetPosVec = XMVectorSet(eyePos.x + XMVectorGetX(normalizeLightDir),
		eyePos.y + XMVectorGetY(normalizeLightDir), eyePos.z + XMVectorGetZ(normalizeLightDir), 1.0f);
	XMVECTOR upVec = XMVectorSet(0.0, 1.0f, 0.0f, 0.0f);
	XMMATRIX lightViewMatrix = XMMatrixLookAtLH(eyePosVec, targetPosVec, upVec);

	return lightViewMatrix;
}