#include "SceneManager.h"
#include "Texture/TextureManager.h"
#include "Common/GraphicsConfig.h"
#include "GameObject/Camera.h"
#include "GameObject/GameObjectManager.h"
#include "SkyBox.h"
#include "Input.h"
#include "WindowInfo.h"
#include "FPS.h"

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{

}

SceneManager::SceneManager(const SceneManager&other)
{

}

shared_ptr<SceneManager> SceneManager::Get()
{
	if (nullptr == single)
	{
		single = shared_ptr<SceneManager>(new SceneManager());
	}

	return single;
}

bool SceneManager::Init()
{
	//TextureManager
	GTextureManager->Init();

	//Camera
	GCamera->SetProjParams(XM_PI / 3.0f, (float)GScreenWidth / (float)GScreenHeight, SCREEN_NEAR, SCREEN_FAR);
	GCamera->SetUIOrthoParams((float)GScreenWidth, (float)GScreenHeight);
	
	//DirectionLight
	shared_ptr<DirectionLight> spDirLight = shared_ptr<DirectionLight>(new DirectionLight());
	spDirLight->SetLightDiretion(XMFLOAT3(-0.5f, -1.0f, 0.0f));
	spDirLight->SetLightColor(XMFLOAT3(0.0f, 0.0f, 0.0f));
	spDirLight->SetLightPostion(XMFLOAT3(10.0f, 10.0f, 10.0f));
	GLightManager->Add(spDirLight);

	//PointLight
	for (int x = 0; x < 20; ++x)
	{
		for (int y = 0; y < 20; ++y)
		{
			float randFloat1 = (float)rand() / (float)RAND_MAX;
			float randFloat2 = (float)rand() / (float)RAND_MAX;
			float randFloat3 = (float)rand() / (float)RAND_MAX;
			float r = randFloat1 >= 0.5 ? 1.0 : 0.0;
			float g = randFloat2 >= 0.5 ? 1.0 : 0.0;
			float b = randFloat3 >= 0.5 ? 1.0 : 0.0;
			if (r == 0 && g == 0 && b == 0)
				r = 1.0;

			float posx = randFloat1 - 0.5;
			float posy = randFloat2 - 0.5;
			shared_ptr<PointLight> pointLight = shared_ptr<PointLight>(new PointLight());
			pointLight->SetLightColor(XMFLOAT3(r, g, b));
			pointLight->SetLightIntensity(6.0f);
			pointLight->SetLightPostion(XMFLOAT3(posx * 200.0f + 70.0f, 2.0f, posy * 200.0f));
			GLightManager->Add(pointLight);
		}
	}

	//Mesh Object
	shared_ptr<Mesh> pHeadMesh = shared_ptr<Mesh>(new Mesh("Resource\\FBXModel\\head\\head.FBX"));
	pHeadMesh->m_eMaterialType = MaterialType::DIFFUSE;
	pHeadMesh->bCastShadow = true;

	shared_ptr<Mesh> pPointLightVolume = shared_ptr<Mesh>(new Mesh("Resource\\FBXModel\\sphere\\sphere.FBX"));
	pPointLightVolume->m_eMaterialType = MaterialType::PURE_COLOR;
	pPointLightVolume->pureColor = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	shared_ptr<Mesh> pTransparentSphereMesh = shared_ptr<Mesh>(new Mesh("Resource\\FBXModel\\sphere\\sphere.FBX"));
	pTransparentSphereMesh->m_eMaterialType = MaterialType::PURE_COLOR;
	pTransparentSphereMesh->bTransparent = true;
	pTransparentSphereMesh->pureColor = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	shared_ptr<Mesh> pSponzaBottom = shared_ptr<Mesh>(new Mesh("Resource\\FBXModel\\sponza\\sponza_bottom.FBX"));
	pSponzaBottom->m_eMaterialType = MaterialType::DIFFUSE;
	pSponzaBottom->bReflect = true;
	pSponzaBottom->bTransparent = false;

	shared_ptr<Mesh> pSponzaNoBottom = shared_ptr<Mesh>(new Mesh("Resource\\FBXModel\\sponza\\sponza_no_bottom.FBX"));
	pSponzaNoBottom->m_eMaterialType = MaterialType::DIFFUSE;

	mHeadObject = shared_ptr<GameObject>(new GameObject());
	mHeadObject->SetMesh(pHeadMesh);
	mHeadObject->m_pTransform->localPosition = XMFLOAT3(0.0f, 5.0f, 0.0f);
	mHeadObject->m_pTransform->localRotation = XMFLOAT3(0.0f, 90.0f, 0.0f);
	mHeadObject->m_pTransform->localScale = XMFLOAT3(5.0f, 5.0f, 5.0f);
	GGameObjectManager->Add(mHeadObject);

	for (int x = 0; x < 11; ++x)
	{
		for (int y = 0; y < 11; ++y)
		{
			shared_ptr<Mesh> pOpacitySphereMesh = shared_ptr<Mesh>(new Mesh("Resource\\FBXModel\\sphere\\sphere.FBX"));
			pOpacitySphereMesh->m_eMaterialType = MaterialType::DIFFUSE;
			pOpacitySphereMesh->pureColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			pOpacitySphereMesh->bCastShadow = false;
			pOpacitySphereMesh->roughness = 0.1f * float(x);
			pOpacitySphereMesh->metal = 0.1f *  float(y);
			mOpacitySphereObject = shared_ptr<GameObject>(new GameObject());
			mOpacitySphereObject->SetMesh(pOpacitySphereMesh);
			mOpacitySphereObject->m_pTransform->localScale = XMFLOAT3(2.0f, 2.0f, 2.0f);
			mOpacitySphereObject->m_pTransform->localPosition = XMFLOAT3(float(x) * 8.0f + 30.0f, 2.0f, float(y) * 8.0f - 30.0f);
			GGameObjectManager->Add(mOpacitySphereObject);
		}
	}

	mSponzaBottom = shared_ptr<GameObject>(new GameObject());
	mSponzaBottom->SetMesh(pSponzaBottom);
	mSponzaBottom->m_pTransform->localPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mSponzaBottom->m_pTransform->localScale = XMFLOAT3(10.0f, 10.0f, 10.0f);

	mSponzaNoBottom = shared_ptr<GameObject>(new GameObject());
	mSponzaNoBottom->SetMesh(pSponzaNoBottom);
	mSponzaNoBottom->m_pTransform->localPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);

	mTransSphereObject = shared_ptr<GameObject>(new GameObject());
	mTransSphereObject->SetMesh(pTransparentSphereMesh);
	mTransSphereObject->m_pTransform->localPosition = XMFLOAT3(0.0f, 20.0f, 0.0f);

	m_pPointVolume = shared_ptr<GameObject>(new GameObject());
	m_pPointVolume->SetMesh(pPointLightVolume);

	GGameObjectManager->Add(mHeadObject);
	GGameObjectManager->Add(mSponzaBottom);
	//GGameObjectManager->Add(mSponzaNoBottom);
	GGameObjectManager->Add(mTransSphereObject);

	//SkyBox
	WCHAR* cubeMapResPath = L"Resource/Texture/night.dds";
	skyBox = shared_ptr<SkyBox>(new SkyBox(cubeMapResPath));

	return true;
}

void SceneManager::Tick(float deltaTime)
{
	bool result;
	int mouseXOffset, mouseYOffset;
	static float rotateY = 0.0f;

	//Tick Input
	result = GInput->Tick();
	if (!result)
	{
		return;
	}

	GFPS->Frame();
	float currentTime = GFPS->GetTime();
	GInput->GetMousePositionOffset(mouseXOffset, mouseYOffset);
	int fps = GFPS->GetFPS();

	//����Ҽ����ڰ��µ�״̬���ܽ��У������ƶ�����ǰ���ƶ�������ת�Ĳ�����
	if (GInput->IsMouseRightButtuonPressed() && fps >= 5 && fps <= 1000000)
	{
		//"W","S"������
		if (GInput->IsWPressed())
		{
			GCamera->Walk(deltaTime*CAMERA_SPEED);
		}
		else if (GInput->IsSPressed())
		{
			GCamera->Walk(-deltaTime * CAMERA_SPEED);
		}

		//"A","D"������
		if (GInput->IsAPressed())
		{
			GCamera->Strafe(-deltaTime * CAMERA_SPEED);
		}
		else if (GInput->IsDPressed())
		{
			GCamera->Strafe(deltaTime*CAMERA_SPEED);
		}

		//"Q","E"������
		if (GInput->IsQPressed())
		{
			GCamera->UpDown(-deltaTime * CAMERA_SPEED);
		}
		else if (GInput->IsEPressed())
		{
			GCamera->UpDown(deltaTime*CAMERA_SPEED);
		}

		//�����ӽ����µ���ת(���տ�ʼ����ת�Ƕ�������90��֮��)
		if (rotateY <= 90.0f && rotateY >= -90.0f)
		{
			rotateY += (float)mouseYOffset*deltaTime;
			GCamera->Pitch((float)mouseYOffset*deltaTime*2.0f);
		}

		//�����ӽ����ҵ���ת
		GCamera->RotateY((float)mouseXOffset*deltaTime*2.0f);
	}

	GCamera->UpdateViewMatrix();

	//TODO:refactor to imgui control
	if (GInput->IsKeyDown(DIK_1))
	{
		GDirectxCore->RecoverDefualtRS();
	}

	if (GInput->IsKeyDown(DIK_2))
	{
		GDirectxCore->TurnOnWireFrameRender();
	}

	if (GInput->IsKeyDown(DIK_0))
	{
		bDebugLightCount = true;
	}
	else
	{
		bDebugLightCount = false;
	}

	//�������ESC�����ƻ�����
	if (GInput->IsEscapePressed())
	{
		DestroyWindow(GHwnd);
	}
}

shared_ptr<SceneManager> SceneManager::single = nullptr;