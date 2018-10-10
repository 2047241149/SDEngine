
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

//������ͶӰ���λ������ռ�㼶AABB��Χ�������
//����һ���󷨣�������ռ��ӽ���㼶�İ˸���������ĵ�(��˸��������ƽ��ֵ)
//�����λ�õ���ƽ�й�Դ��λ��
XMMATRIX DirectionLight::GetViewMatrix()
{
	/*float fNearPlane = GCamera->mNearPlane;
	float fFarPlane = GCamera->mFarPlane;
	float fFov = GCamera->mFovY;
	float fAspect = GCamera->mAspect;
	float fNearY = tan(fFov / 2.0f) * fNearPlane;
	float fNearX = fNearY / fAspect;
	float fFarY = tan(fFov / 2.0f) * fFarPlane;
	float fFarX = fFarY / fAspect;

	const int FRUSTUN_VERTEX_NUM = 8;
	XMVECTOR frustumPoint[FRUSTUN_VERTEX_NUM];
	frustumPoint[0] = XMVectorSet(fNearX, fNearY, fNearPlane, 1.0f); //��ƽ�����Ͻ�
	frustumPoint[1] = XMVectorSet(fNearX, -fNearY, fNearPlane, 1.0f); //��ƽ�����½�
	frustumPoint[2] = XMVectorSet(-fNearX, fNearY, fNearPlane, 1.0f); //��ƽ�����Ͻ�
	frustumPoint[3] = XMVectorSet(-fNearX, -fNearY, fNearPlane, 1.0f); //��ƽ�����½�
	frustumPoint[4] = XMVectorSet(fFarX, fFarY, fFarPlane, 1.0f); //Զƽ�����Ͻ�
	frustumPoint[5] = XMVectorSet(fFarX, -fFarY, fFarPlane, 1.0f); //Զƽ�����½�
	frustumPoint[6] = XMVectorSet(-fFarX, fFarY, fFarPlane, 1.0f); //Զƽ�����Ͻ�
	frustumPoint[7] = XMVectorSet(-fFarX, -fFarY, fFarPlane, 1.0f); //Զƽ�����½�


	XMMATRIX viewMatrix = GCamera->GetViewMatrix();
	XMVECTOR det = XMMatrixDeterminant(viewMatrix);
	XMMATRIX invenseViewMatrix = XMMatrixInverse(&det, viewMatrix);
	XMVECTOR wsSceneAABBMax = XMVectorSet(FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN);
	XMVECTOR wsSceneAABBMin = XMVectorSet(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
	XMVECTOR wsScenePointAvage = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	//��������ӽ���İ˸������������ռ�任������ռ�
	//����˸�����������ռ��ƽ��ֵ(���ĵ�)
	for (int index = 0; index < FRUSTUN_VERTEX_NUM; ++index)
	{
		frustumPoint[index] = XMVector3TransformCoord(frustumPoint[index], invenseViewMatrix);
		wsSceneAABBMax = XMVectorMax(wsSceneAABBMax, frustumPoint[index]);
		wsSceneAABBMin = XMVectorMin(wsSceneAABBMin, frustumPoint[index]);
		wsScenePointAvage += frustumPoint[index];
	}


	XMFLOAT3 f3SceneAABBPointAvage;
	XMStoreFloat3(&f3SceneAABBPointAvage, wsScenePointAvage);

	XMFLOAT3 eyePos = XMFLOAT3(f3SceneAABBPointAvage.x / (float)FRUSTUN_VERTEX_NUM,
		f3SceneAABBPointAvage.y / (float)FRUSTUN_VERTEX_NUM, f3SceneAABBPointAvage.z / (float)FRUSTUN_VERTEX_NUM);*/
	XMFLOAT3 eyePos = GCamera->GetPosition();
	XMVECTOR eyePosVec = XMVectorSet(eyePos.x, eyePos.y, eyePos.z, 1.0f);
	XMVECTOR targetPosVec = XMVectorSet(eyePos.x + m_DirLightDiretion.x, eyePos.y + m_DirLightDiretion.y, eyePos.z + m_DirLightDiretion.z, 1.0f);

	XMVECTOR upVec = XMVectorSet(0.0, 1.0f, 0.0f, 0.0f);
	XMMATRIX lightViewMatrix = XMMatrixLookAtLH(eyePosVec, targetPosVec, upVec);

	return lightViewMatrix;
}

