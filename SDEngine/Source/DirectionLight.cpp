
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

void DirectionLight::GetDirLightViewAndProjMatrix(XMMATRIX& lightViewMatrix, XMMATRIX& lightOrthoProjMatrix)
{
	float fNearPlane = GCamera->mNearPlane;
	float fFarPlane = GCamera->mFarPlane;
	float fFov = GCamera->mFovY;
	float fAspect = GCamera->mAspect;
	float fNearY = tan(fFov / 2.0f) * fNearPlane;
	float fNearX = fNearY / fAspect;
	float fFarY = tan(fFov / 2.0f) * fFarPlane;
	float fFarX = fFarY / fAspect;
	
	const int FRUSTUN_VERTEX_NUM = 8;
	XMVECTOR frustumPoint[FRUSTUN_VERTEX_NUM];
	frustumPoint[0] = XMVectorSet(fNearX, fNearY, fNearPlane, 1.0f); //近平面右上角
	frustumPoint[1] = XMVectorSet(fNearX, -fNearY, fNearPlane, 1.0f); //近平面左下角
	frustumPoint[2] = XMVectorSet(-fNearX, fNearY, fNearPlane, 1.0f); //近平面左上角
	frustumPoint[3] = XMVectorSet(-fNearX, -fNearY, fNearPlane, 1.0f); //近平面左下角
	frustumPoint[4] = XMVectorSet(fFarX, fFarY, fFarPlane, 1.0f); //远平面右上角
	frustumPoint[5] = XMVectorSet(fFarX, -fFarY, fFarPlane, 1.0f); //远平面左下角
	frustumPoint[6] = XMVectorSet(-fFarX, fFarY, fFarPlane, 1.0f); //远平面左上角
	frustumPoint[7] = XMVectorSet(-fFarX, -fFarY, fFarPlane, 1.0f); //远平面左下角


	XMMATRIX viewMatrix = GCamera->GetViewMatrix();
	XMVECTOR det = XMMatrixDeterminant(viewMatrix);
	XMMATRIX invenseViewMatrix = XMMatrixInverse(&det, viewMatrix);
	XMVECTOR wsSceneAABBMax = XMVectorSet(FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN);
	XMVECTOR wsSceneAABBMin = XMVectorSet(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);

	//将主相机视截体的八个顶点从主相机空间变换到世界空间
	for (int index = 0; index < FRUSTUN_VERTEX_NUM; ++index)
	{
		frustumPoint[index] = XMVector3TransformCoord(frustumPoint[index], invenseViewMatrix);
	}

	//求这八个顶点在世界空间的AABB包围盒
	for (int index = 0; index < FRUSTUN_VERTEX_NUM; ++index)
	{
		wsSceneAABBMax = XMVectorMax(wsSceneAABBMax, frustumPoint[index]);
		wsSceneAABBMin = XMVectorMin(wsSceneAABBMin, frustumPoint[index]);
	}

	XMFLOAT3 f3SceneAABBMax; 
	XMFLOAT3 f3SceneAABBMin;
	XMStoreFloat3(&f3SceneAABBMax, wsSceneAABBMax);
	XMStoreFloat3(&f3SceneAABBMin, wsSceneAABBMin);
	
	XMFLOAT3 eyePos = XMFLOAT3((f3SceneAABBMin.x + f3SceneAABBMax.x) / 2.0f,
		(f3SceneAABBMin.y + f3SceneAABBMax.y) / 2.0f, (f3SceneAABBMin.z + f3SceneAABBMax.z) / 2.0f);

	eyePos = XMFLOAT3(60.0f, 60.0f, 1.0f);
	XMVECTOR eyePosVec = XMVectorSet(eyePos.x, eyePos.y, eyePos.z, 1.0f);
	XMVECTOR targetPosVec = XMVectorSet(eyePos.x + m_DirLightDiretion.x, eyePos.y + m_DirLightDiretion.y, eyePos.z + m_DirLightDiretion.z, 1.0f);

	XMVECTOR upVec = XMVectorSet(0.0, 1.0f, 0.0f, 0.0f);
	lightViewMatrix = XMMatrixLookAtLH(eyePosVec, targetPosVec, upVec);

	//将主相机视截体的八个顶点变换到光源相机空间
	for (int index = 0; index < FRUSTUN_VERTEX_NUM; ++index)
	{
		frustumPoint[index] = XMVector3TransformCoord(frustumPoint[index], lightViewMatrix);
	}

	XMVECTOR lightVsSceneAABBMax = XMVectorSet(FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN);
	XMVECTOR lightVsSceneAABBMin = XMVectorSet(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);

	//求这八个顶点在光源相机空间的AABB包围盒
	for (int index = 0; index < FRUSTUN_VERTEX_NUM; ++index)
	{
		lightVsSceneAABBMax = XMVectorMax(lightVsSceneAABBMax, frustumPoint[index]);
		lightVsSceneAABBMin = XMVectorMin(lightVsSceneAABBMin, frustumPoint[index]);
	}

	XMFLOAT3 f3LightVsSceneAABBMax;
	XMFLOAT3 f3LightVsSceneAABBMin;
	XMStoreFloat3(&f3LightVsSceneAABBMax, lightVsSceneAABBMax);
	XMStoreFloat3(&f3LightVsSceneAABBMin, lightVsSceneAABBMin);

	/*lightOrthoProjMatrix = XMMatrixOrthographicOffCenterLH
	(
		f3LightVsSceneAABBMin.x, f3LightVsSceneAABBMax.x,
		f3LightVsSceneAABBMin.y, f3LightVsSceneAABBMax.y,
		f3LightVsSceneAABBMin.z, f3LightVsSceneAABBMax.z
	);*/

	lightOrthoProjMatrix = XMMatrixOrthographicLH(200.0f, 200.0f, 1.0f, 300.0f);

}

