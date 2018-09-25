#include "CascadeShadowMapManager.h"
#include "Camera.h"

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
	for (int index = 0; index < CASCADE_SHADOW_MAP_NUM; ++index)
	{
		mArrayShadowMap[index] = shared_ptr<ShadowMap>(new ShadowMap(nShadowMapResolution, nShadowMapResolution));
	}
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
		float fNearY = tan(fFov / 2.0f) * fNearPlane;
		float fNearX = fNearY / fAspect;

		float fFarPlane = (GCamera->mFarPlane - GCamera->mNearPlane) * CASCADE_PERCENT[nCascadeindex + 1] + GCamera->mNearPlane;
		mfCameraZ[nCascadeindex] = fFarPlane;
		float fFarY = tan(fFov / 2.0f) * fFarPlane;
		float fFarX = fFarY / fAspect;

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

		//��������ӽ���İ˸������������ռ�任������ռ�
		for (int nFrustumindex = 0; nFrustumindex < FRUSTUN_VERTEX_NUM; ++nFrustumindex)
		{
			frustumPoint[nFrustumindex] = XMVector3TransformCoord(frustumPoint[nFrustumindex], invenseViewMatrix);
		}


		//��������ӽ���İ˸�����任����Դ����ռ�
		for (int nFrustumindex = 0; nFrustumindex < FRUSTUN_VERTEX_NUM; ++nFrustumindex)
		{
			frustumPoint[nFrustumindex] = XMVector3TransformCoord(frustumPoint[nFrustumindex], lightViewMatrix);
		}

		XMVECTOR lightVsSceneAABBMax = XMVectorSet(FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN);
		XMVECTOR lightVsSceneAABBMin = XMVectorSet(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);

		//����˸������ڹ�Դ����ռ��AABB��Χ��
		for (int nFrustumindex = 0; nFrustumindex < FRUSTUN_VERTEX_NUM; ++nFrustumindex)
		{
			lightVsSceneAABBMax = XMVectorMax(lightVsSceneAABBMax, frustumPoint[nFrustumindex]);
			lightVsSceneAABBMin = XMVectorMin(lightVsSceneAABBMin, frustumPoint[nFrustumindex]);
		}

		XMFLOAT3 f3LightVsSceneAABBMax;
		XMFLOAT3 f3LightVsSceneAABBMin;
		XMStoreFloat3(&f3LightVsSceneAABBMax, lightVsSceneAABBMax);
		XMStoreFloat3(&f3LightVsSceneAABBMin, lightVsSceneAABBMin);

		mArrayLightOrthoMatrix[nCascadeindex] = XMMatrixOrthographicOffCenterLH
		(
			f3LightVsSceneAABBMin.x, f3LightVsSceneAABBMax.x,
			f3LightVsSceneAABBMin.y, f3LightVsSceneAABBMax.y,
			f3LightVsSceneAABBMin.z, f3LightVsSceneAABBMax.z
		);
	}	
}

void CascadedShadowsManager::SetRenderTarget(int index)
{
	mArrayShadowMap[index]->SetRenderTarget();
}

ID3D11ShaderResourceView* CascadedShadowsManager::GetShadowMapSRV(int index)
{
	return mArrayShadowMap[index]->GetShadowMap();
}


