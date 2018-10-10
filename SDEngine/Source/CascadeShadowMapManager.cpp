#include "CascadeShadowMapManager.h"
#include "Camera.h"
#include "CascadeShadowMap.h"

CascadedShadowsManager::CascadedShadowsManager(int nShadowMapResolution)
{
	InitShadowMap(nShadowMapResolution);
}


CascadedShadowsManager::CascadedShadowsManager(const CascadedShadowsManager& other)
{

}
CascadedShadowsManager::~CascadedShadowsManager()
{

}

void CascadedShadowsManager::InitShadowMap(int nShadowMapResolution)
{

	mCascadeShadowMap = shared_ptr<CascadeShadowMap>(new CascadeShadowMap(nShadowMapResolution, nShadowMapResolution, CASCADE_SHADOW_MAP_NUM));
	
}

void CascadedShadowsManager::Update()
{
	float fFov = GCamera->mFovY;
	float fAspect = GCamera->mAspect;
	const int FRUSTUN_VERTEX_NUM = 8;
	XMVECTOR frustumPoint[FRUSTUN_VERTEX_NUM];
	shared_ptr<DirectionLight> mMainDirLight = GLightManager->GetMainLight();
	XMMATRIX lightViewMatrix = mMainDirLight->GetViewMatrix();

	for (int nCascadeindex = 0; nCascadeindex < CASCADE_SHADOW_MAP_NUM; ++nCascadeindex)
	{

		float fNearPlane = (GCamera->mFarPlane - GCamera->mNearPlane) * CASCADE_PERCENT[nCascadeindex] + GCamera->mNearPlane;
		//float fNearPlane =  GCamera->mNearPlane;
		float fNearY = tan(fFov / 2.0f) * fNearPlane;
		float fNearX = fNearY / fAspect;

		float fFarPlane = (GCamera->mFarPlane - GCamera->mNearPlane) * CASCADE_PERCENT[nCascadeindex + 1] + GCamera->mNearPlane;
		mfCameraZ[nCascadeindex] = fFarPlane;
		float fFarY = tan(fFov / 2.0f) * fFarPlane;
		float fFarX = fFarY / fAspect;

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
		XMVECTOR lightVsSceneAABBMax = XMVectorSet(FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN);
		XMVECTOR lightVsSceneAABBMin = XMVectorSet(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
		XMVECTOR tempFrustumPoint;

		// 将主相机视截体的八个顶点从主相机空间变换到世界空间
		for (int nFrustumindex = 0; nFrustumindex < FRUSTUN_VERTEX_NUM; ++nFrustumindex)
		{
			frustumPoint[nFrustumindex] = XMVector3TransformCoord(frustumPoint[nFrustumindex], invenseViewMatrix);
			wsSceneAABBMax = XMVectorMax(wsSceneAABBMax, frustumPoint[nFrustumindex]);
			wsSceneAABBMin = XMVectorMin(wsSceneAABBMin, frustumPoint[nFrustumindex]);
		}

		// 将主相机视截体的八个顶点变换到光源相机空间
		for (int nFrustumindex = 0; nFrustumindex < FRUSTUN_VERTEX_NUM; ++nFrustumindex)
		{
			tempFrustumPoint = XMVector3TransformCoord(frustumPoint[nFrustumindex], lightViewMatrix);
			lightVsSceneAABBMax = XMVectorMax(lightVsSceneAABBMax, tempFrustumPoint);
			lightVsSceneAABBMin = XMVectorMin(lightVsSceneAABBMin, tempFrustumPoint);
		}

		//在XY范围上，谨记一点，lightVsSceneAABBMax和lightVsSceneAABBMin由于形变原因并不一定包围整个视截体范围，我们得扩大lightVsSceneAABB的范围
		//lightVsSceneAABB的范围应该以世界空间的包围范围为准，因此核对世界空间的梯形对角线大小
		XMVECTOR vDiagonal = wsSceneAABBMax - wsSceneAABBMin;
		vDiagonal = XMVector3Length(vDiagonal);
		XMVECTOR vBorderoffset = (vDiagonal - (lightVsSceneAABBMax - lightVsSceneAABBMin)) * XMVectorSet(0.5f, 0.5f, 0.0, 0.0f);

		//防止vBorderoffset的X或Y为负数的情况下，可能产生lightVsSceneAABBMin > lightVsSceneAABBMax的结果
		XMVECTOR vZero = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		lightVsSceneAABBMax += XMVectorMax(vZero,vBorderoffset);
		lightVsSceneAABBMin -= XMVectorMax(vZero, vBorderoffset);


		XMFLOAT3 f3LightVsSceneAABBMax;
		XMFLOAT3 f3LightVsSceneAABBMin;
		XMStoreFloat3(&f3LightVsSceneAABBMax, lightVsSceneAABBMax);
		XMStoreFloat3(&f3LightVsSceneAABBMin, lightVsSceneAABBMin);

		//这里Z远近距离省略了整个场景AABB包围盒的求交(过程略为复杂)，直接回退GCamera->mFarPlane * 0.5f
		mArrayLightOrthoMatrix[nCascadeindex] = XMMatrixOrthographicOffCenterLH
		(
			f3LightVsSceneAABBMin.x, f3LightVsSceneAABBMax.x,
			f3LightVsSceneAABBMin.y, f3LightVsSceneAABBMax.y,
			f3LightVsSceneAABBMin.z - GCamera->mFarPlane * 0.5f, f3LightVsSceneAABBMax.z
		);
	}	
}

void CascadedShadowsManager::SetRenderTarget(int nCascadeIndex)
{
	mCascadeShadowMap->SetRenderTarget(nCascadeIndex);
}

ID3D11ShaderResourceView* CascadedShadowsManager::GetShadowMapSRV()
{
	return mCascadeShadowMap->GetShadowMap();
}

void CascadedShadowsManager::ClearDepthBuffer()
{
	mCascadeShadowMap->ClearDepthBuffer();
}
