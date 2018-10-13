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

		float fNearPlane = 0;
		float fFarPlane = GCamera->mFarPlane * CASCADE_PERCENT[nCascadeindex + 1];
		mfCameraZ[nCascadeindex] = fFarPlane;
		float fFarY = tan(fFov / 2.0f) * fFarPlane;
		float fFarX = fFarY / fAspect;

		frustumPoint[0] = XMVectorSet(fNearPlane, fNearPlane, fNearPlane, 1.0f); //近平面右上角
		frustumPoint[1] = XMVectorSet(fNearPlane, fNearPlane, fNearPlane, 1.0f); //近平面右下角
		frustumPoint[2] = XMVectorSet(fNearPlane, fNearPlane, fNearPlane, 1.0f); //近平面左上角
		frustumPoint[3] = XMVectorSet(fNearPlane, fNearPlane, fNearPlane, 1.0f); //近平面左下角
		frustumPoint[4] = XMVectorSet(fFarX, fFarY, fFarPlane, 1.0f); //远平面右上角
		frustumPoint[5] = XMVectorSet(fFarX, -fFarY, fFarPlane, 1.0f); //远平面右下角
		frustumPoint[6] = XMVectorSet(-fFarX, fFarY, fFarPlane, 1.0f); //远平面左上角
		frustumPoint[7] = XMVectorSet(-fFarX, -fFarY, fFarPlane, 1.0f); //远平面左下角


		XMMATRIX viewMatrix = GCamera->GetViewMatrix();
		XMMATRIX invenseViewMatrix = XMMatrixInverse(nullptr, viewMatrix);
		XMVECTOR lightVsSceneAABBMax = XMVectorSet(FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN);
		XMVECTOR lightVsSceneAABBMin = XMVectorSet(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
		XMVECTOR tempFrustumPoint;

		// 将主相机视截体的八个顶点从主相机空间变换到世界空间
		for (int nFrustumindex = 0; nFrustumindex < FRUSTUN_VERTEX_NUM; ++nFrustumindex)
		{
			frustumPoint[nFrustumindex] = XMVector3TransformCoord(frustumPoint[nFrustumindex], invenseViewMatrix);
		}

		// 将主相机视截体的八个顶点变换到光源相机空间
		for (int nFrustumindex = 0; nFrustumindex < FRUSTUN_VERTEX_NUM; ++nFrustumindex)
		{
			tempFrustumPoint = XMVector3TransformCoord(frustumPoint[nFrustumindex], lightViewMatrix);
			lightVsSceneAABBMax = XMVectorMax(lightVsSceneAABBMax, tempFrustumPoint);
			lightVsSceneAABBMin = XMVectorMin(lightVsSceneAABBMin, tempFrustumPoint);
		}

		float fLightVsSceneAABBMinZ = XMVectorGetZ(lightVsSceneAABBMin);
		float fLightVsSceneAABBMaxZ = XMVectorGetZ(lightVsSceneAABBMax);

		//在XY范围上，谨记一点，lightVsSceneAABBMax和lightVsSceneAABBMin由于形变原因并不一定包围整个视截体范围，我们得扩大lightVsSceneAABB的范围
		//lightVsSceneAABB的范围应该以世界空间的包围范围为准，因此核对世界空间的梯形对角线大小
		XMVECTOR vDiagonal = frustumPoint[0] - frustumPoint[7];
		vDiagonal = XMVector3Length(vDiagonal);
		float fCascadeDiagonal = XMVectorGetX(vDiagonal);
		XMVECTOR vBorderoffset = (vDiagonal - (lightVsSceneAABBMax - lightVsSceneAABBMin)) * XMVectorSet(0.5f, 0.5f, 0.0, 0.0f);

		//防止vBorderoffset的X或Y为负数的情况下，可能产生lightVsSceneAABBMin > lightVsSceneAABBMax的结果
		//XMVECTOR vZero = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		lightVsSceneAABBMax += vBorderoffset;
		lightVsSceneAABBMin -= vBorderoffset;


		//纠正lightVsSceneAABBMax和lightVsSceneAABBMin的大小，对应ShadowMap的
		//(1)世界空间的OrthoViewFrustum大小应该ShadowMap大小的整数倍
		//fWorldUnitPerTexel -->OrthoViewFrustum 每个ShadowMap纹素对应多少个世界单位
		float fWorldUnitPerTexel = fCascadeDiagonal / (float)SHADOW_MAP_SIZE;
		XMVECTOR vWorldUnitPerTexel = XMVectorSet(fWorldUnitPerTexel, fWorldUnitPerTexel, 0, 0);

		//(2)lightVsSceneAABBMax，lightVsSceneAABBMin对应到整数个fWorldUnitPerTexel单位
		//综上(1)(2)ightVsSceneAABBMax，lightVsSceneAABBMin就可以对应整数个ShadowMap的纹素,
		//就消除了之前浮点数个ShadowMap纹素造成的闪烁问题
		lightVsSceneAABBMin /= vWorldUnitPerTexel;
		lightVsSceneAABBMin = XMVectorFloor(lightVsSceneAABBMin);
		lightVsSceneAABBMin *= vWorldUnitPerTexel;

		lightVsSceneAABBMax /= vWorldUnitPerTexel;
		lightVsSceneAABBMax = XMVectorFloor(lightVsSceneAABBMax);
		lightVsSceneAABBMax *= vWorldUnitPerTexel;


		XMFLOAT3 f3LightVsSceneAABBMax;
		XMFLOAT3 f3LightVsSceneAABBMin;
		XMStoreFloat3(&f3LightVsSceneAABBMax, lightVsSceneAABBMax);
		XMStoreFloat3(&f3LightVsSceneAABBMin, lightVsSceneAABBMin);

		//这里Z远近距离省略了整个场景AABB包围盒的求交(过程略为复杂)，直接回退GCamera->mFarPlane * 0.5f
		mArrayLightOrthoMatrix[nCascadeindex] = XMMatrixOrthographicOffCenterLH
		(
			f3LightVsSceneAABBMin.x, f3LightVsSceneAABBMax.x,
			f3LightVsSceneAABBMin.y, f3LightVsSceneAABBMax.y,
			fLightVsSceneAABBMinZ - GCamera->mFarPlane, fLightVsSceneAABBMaxZ
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
