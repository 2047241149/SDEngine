#include "CascadeShadowMapManager.h"
#include "GameObject/Camera.h"
#include "CascadeShadowMap.h"
#include "Light/LightManager.h"
#include "Light/DirectionLight.h"

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

		frustumPoint[0] = XMVectorSet(fNearPlane, fNearPlane, fNearPlane, 1.0f); //��ƽ�����Ͻ�
		frustumPoint[1] = XMVectorSet(fNearPlane, fNearPlane, fNearPlane, 1.0f); //��ƽ�����½�
		frustumPoint[2] = XMVectorSet(fNearPlane, fNearPlane, fNearPlane, 1.0f); //��ƽ�����Ͻ�
		frustumPoint[3] = XMVectorSet(fNearPlane, fNearPlane, fNearPlane, 1.0f); //��ƽ�����½�
		frustumPoint[4] = XMVectorSet(fFarX, fFarY, fFarPlane, 1.0f); //Զƽ�����Ͻ�
		frustumPoint[5] = XMVectorSet(fFarX, -fFarY, fFarPlane, 1.0f); //Զƽ�����½�
		frustumPoint[6] = XMVectorSet(-fFarX, fFarY, fFarPlane, 1.0f); //Զƽ�����Ͻ�
		frustumPoint[7] = XMVectorSet(-fFarX, -fFarY, fFarPlane, 1.0f); //Զƽ�����½�


		XMMATRIX viewMatrix = GCamera->GetViewMatrix();
		XMMATRIX invenseViewMatrix = XMMatrixInverse(nullptr, viewMatrix);
		XMVECTOR lightVsSceneAABBMax = XMVectorSet(FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN);
		XMVECTOR lightVsSceneAABBMin = XMVectorSet(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
		XMVECTOR tempFrustumPoint;

		// ��������ӽ���İ˸������������ռ�任������ռ�
		for (int nFrustumindex = 0; nFrustumindex < FRUSTUN_VERTEX_NUM; ++nFrustumindex)
		{
			frustumPoint[nFrustumindex] = XMVector3TransformCoord(frustumPoint[nFrustumindex], invenseViewMatrix);
		}

		// ��������ӽ���İ˸�����任����Դ����ռ�
		for (int nFrustumindex = 0; nFrustumindex < FRUSTUN_VERTEX_NUM; ++nFrustumindex)
		{
			tempFrustumPoint = XMVector3TransformCoord(frustumPoint[nFrustumindex], lightViewMatrix);
			lightVsSceneAABBMax = XMVectorMax(lightVsSceneAABBMax, tempFrustumPoint);
			lightVsSceneAABBMin = XMVectorMin(lightVsSceneAABBMin, tempFrustumPoint);
		}

		float fLightVsSceneAABBMinZ = XMVectorGetZ(lightVsSceneAABBMin);
		float fLightVsSceneAABBMaxZ = XMVectorGetZ(lightVsSceneAABBMax);

		//��XY��Χ�ϣ�����һ�㣬lightVsSceneAABBMax��lightVsSceneAABBMin�����α�ԭ�򲢲�һ����Χ�����ӽ��巶Χ�����ǵ�����lightVsSceneAABB�ķ�Χ
		//lightVsSceneAABB�ķ�ΧӦ��������ռ�İ�Χ��ΧΪ׼����˺˶�����ռ�����ζԽ��ߴ�С
		XMVECTOR vDiagonal = frustumPoint[0] - frustumPoint[7];
		vDiagonal = XMVector3Length(vDiagonal);
		float fCascadeDiagonal = XMVectorGetX(vDiagonal);
		XMVECTOR vBorderoffset = (vDiagonal - (lightVsSceneAABBMax - lightVsSceneAABBMin)) * XMVectorSet(0.5f, 0.5f, 0.0, 0.0f);

		//��ֹvBorderoffset��X��YΪ����������£����ܲ���lightVsSceneAABBMin > lightVsSceneAABBMax�Ľ��
		//XMVECTOR vZero = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		lightVsSceneAABBMax += vBorderoffset;
		lightVsSceneAABBMin -= vBorderoffset;


		//����lightVsSceneAABBMax��lightVsSceneAABBMin�Ĵ�С����ӦShadowMap��
		//(1)����ռ��OrthoViewFrustum��СӦ��ShadowMap��С��������
		//fWorldUnitPerTexel -->OrthoViewFrustum ÿ��ShadowMap���ض�Ӧ���ٸ����絥λ
		float fWorldUnitPerTexel = fCascadeDiagonal / (float)SHADOW_MAP_SIZE;
		XMVECTOR vWorldUnitPerTexel = XMVectorSet(fWorldUnitPerTexel, fWorldUnitPerTexel, 0, 0);

		//(2)lightVsSceneAABBMax��lightVsSceneAABBMin��Ӧ��������fWorldUnitPerTexel��λ
		//����(1)(2)ightVsSceneAABBMax��lightVsSceneAABBMin�Ϳ��Զ�Ӧ������ShadowMap������,
		//��������֮ǰ��������ShadowMap������ɵ���˸����
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

		//����ZԶ������ʡ������������AABB��Χ�е���(������Ϊ����)��ֱ�ӻ���GCamera->mFarPlane * 0.5f
		//ǰ��GCamera->mFarPlane * 0.15����OthrhoԶ��ZBuffer 1.0��ͻ
		mArrayLightOrthoMatrix[nCascadeindex] = XMMatrixOrthographicOffCenterLH
		(
			f3LightVsSceneAABBMin.x, f3LightVsSceneAABBMax.x,
			f3LightVsSceneAABBMin.y, f3LightVsSceneAABBMax.y,
			fLightVsSceneAABBMinZ - GCamera->mFarPlane * 0.5f, fLightVsSceneAABBMaxZ + GCamera->mFarPlane * 0.15f
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
