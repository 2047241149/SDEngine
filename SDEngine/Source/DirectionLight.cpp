
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

//方向光的投影相机位于世界空间层级AABB包围体的中心
//另外一种求法：在世界空间视截体层级的八个顶点的中心点(求八个点坐标的平均值)
//相机的位置当作平行光源的位置
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
	XMVECTOR wsScenePointAvage = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	//将主相机视截体的八个顶点从主相机空间变换到世界空间
	//求这八个顶点在世界空间的平均值(中心点)
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

