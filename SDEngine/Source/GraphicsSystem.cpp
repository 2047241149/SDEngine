#include "GraphicsSystem.h"
#include "Texture/TextureSamplerManager.h"
#include "SkyBox.h"
#include "GameObject/Camera.h"
#include "GameObject/GameObjectManager.h"
#include "Texture/RenderTexture.h"
#include "Input.h"
#include "Shader/CascadeShadowMapManager.h"
#include "GameObject/Quad.h"
#include "DebugTool/Debugwindow.h"
#include "Shader/GeometryBuffer.h"
#include "Shader/SSRGBuffer.h"
#include "Shader/DepthBufferRT.h"
#include "Shader/SSAOManager.h"
#include "Texture/TextureManager.h"

GraphicsSystem::GraphicsSystem(int ScreenWidth, int ScreenHeight, HWND hwnd, HINSTANCE hinstance)
{
	Init(ScreenWidth, ScreenHeight, hwnd, hinstance);
}

GraphicsSystem::~GraphicsSystem()
{
	CloseDebugConsole();
}

GraphicsSystem::GraphicsSystem(const GraphicsSystem&other)
{

}

bool GraphicsSystem::Init(int ScreenWidth, int ScreenHeight, HWND hwnd,HINSTANCE hinstance)
{

	mhwnd = hwnd;

	m_nScreenWidth = ScreenWidth;
	m_nScreenHeight = ScreenHeight;

	//创建D3DClass类并且初始化,D3DClass是第一个被创建并且初始化
	GDirectxCore->Init(ScreenWidth, ScreenHeight, VSYNC_ENABLE, hwnd, FULL_SCREEN, SCREEN_NEAR, SCREEN_FAR);

	//Console的Debug窗口
	InitDebugConsole();

	GTextureManager->Init();

	//创建相机
	GCamera->SetProjParams(XM_PI / 3.0f, (float)ScreenWidth / (float)ScreenHeight, SCREEN_NEAR, SCREEN_FAR);
	GCamera->SetUIOrthoParams((float)ScreenWidth, (float)ScreenHeight);

	//给游戏添加灯光 
	shared_ptr<DirectionLight> m_spDirLight = shared_ptr<DirectionLight>(new DirectionLight());
	m_spDirLight->SetLightDiretion(XMFLOAT3(-0.5f, -1.0f, 0.0f));
	m_spDirLight->SetLightColor(XMFLOAT3(1.0f, 1.0f, 1.0f));
	m_spDirLight->SetLightPostion(XMFLOAT3(10.0f, 10.0f, 10.0f));
	GLightManager->Add(m_spDirLight);

	shared_ptr<PointLight> m_PointLight = shared_ptr<PointLight>(new PointLight());
	m_PointLight->SetLightColor(XMFLOAT3(1.0f, 1.0f, 1.0f));
	m_PointLight->SetLightIntensity(5.0f);
	m_PointLight->SetLightPostion(XMFLOAT3(20.0f, 1.0f, -20.0f));

	shared_ptr<PointLight> m_PointLight1 = shared_ptr<PointLight>(new PointLight());
	m_PointLight1->SetLightColor(XMFLOAT3(1.0f, 0.0f, 0.0f));
	m_PointLight1->SetLightIntensity(5.0f);
	m_PointLight1->SetLightPostion(XMFLOAT3(20.0f, 1.0f, 20.0f));

	GLightManager->Add(m_PointLight);
	GLightManager->Add(m_PointLight1);
	
	//创建mesh
	//(1)
	shared_ptr<Mesh> pHeadMesh = shared_ptr<Mesh>(new Mesh("Resource\\FBXModel\\head\\head.FBX"));
	pHeadMesh->m_eMaterialType = MaterialType::DIFFUSE;
	pHeadMesh->bCastShadow = true;

	shared_ptr<Mesh> pPointLightVolume = shared_ptr<Mesh>(new Mesh("Resource\\FBXModel\\sphere\\sphere.FBX"));
	pPointLightVolume->m_eMaterialType = MaterialType::PURE_COLOR;
	pPointLightVolume->pureColor = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	shared_ptr<Mesh>  pTransparentSphereMesh = shared_ptr<Mesh>(new Mesh("Resource\\FBXModel\\sphere\\sphere.FBX"));
	pTransparentSphereMesh->m_eMaterialType = MaterialType::PURE_COLOR;
	pTransparentSphereMesh->bTransparent = true;
	pTransparentSphereMesh->pureColor = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	shared_ptr<Mesh> pSponzaBottom = shared_ptr<Mesh>(new Mesh("Resource\\FBXModel\\sponza\\sponza_bottom.FBX"));
	pSponzaBottom->m_eMaterialType = MaterialType::DIFFUSE;
	pSponzaBottom->bReflect = true;
	pSponzaBottom->bTransparent = false;

	shared_ptr<Mesh> pSponzaNoBottom = shared_ptr<Mesh>(new Mesh("Resource\\FBXModel\\sponza\\sponza_no_bottom.FBX"));
	pSponzaNoBottom->m_eMaterialType = MaterialType::DIFFUSE;

	//创建GameObject
	mHeadObject = shared_ptr<GameObject>(new GameObject());
	mHeadObject->SetMesh(pHeadMesh);
	mHeadObject->m_pTransform->localPosition = XMFLOAT3(0.0f, 5.0f, 0.0f);
	mHeadObject->m_pTransform->localRotation = XMFLOAT3(0.0f, 90.0f, 0.0f);
	mHeadObject->m_pTransform->localScale = XMFLOAT3(5.0f, 5.0f, 5.0f);


	for (int x = 0; x < 11; ++x)
	{
		for (int y = 0; y < 11; ++y)
		{
			shared_ptr<Mesh> pOpacitySphereMesh = shared_ptr<Mesh>(new Mesh("Resource\\FBXModel\\sphere\\sphere.FBX"));
			pOpacitySphereMesh->m_eMaterialType = MaterialType::DIFFUSE;
			pOpacitySphereMesh->pureColor = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
			pOpacitySphereMesh->bCastShadow = false;
			pOpacitySphereMesh->roughness = 0.1f * float(x);
			pOpacitySphereMesh->metal = 0.1f *  float(y);
			mOpacitySphereObject = shared_ptr<GameObject>(new GameObject());
			mOpacitySphereObject->SetMesh(pOpacitySphereMesh);
			mOpacitySphereObject->m_pTransform->localScale = XMFLOAT3(1.0f, 1.0f, 1.0f);
			mOpacitySphereObject->m_pTransform->localPosition = XMFLOAT3(5.0f, float(y) * 2.5f + 10.0f, float(x) * 2.5f);
			GGameObjectManager->Add(mOpacitySphereObject);
		}
	}


	mSponzaBottom = shared_ptr<GameObject>(new GameObject());
	mSponzaBottom->SetMesh(pSponzaBottom);
	mSponzaBottom->m_pTransform->localPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);

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

	//创建输入类
	mInputClass = shared_ptr<Input>(new Input(hinstance, hwnd, ScreenWidth, ScreenHeight));

	mSrcRT = shared_ptr<RenderTexture>(
		new RenderTexture(ScreenWidth, ScreenHeight));

	mSSRRT = shared_ptr<RenderTexture>(
		new RenderTexture(ScreenWidth, ScreenHeight));

	mCascadeShadowsManager = shared_ptr<CascadedShadowsManager>(new CascadedShadowsManager(SHADOW_MAP_SIZE));

	mGrayShadowMap = shared_ptr<RenderTexture>(new RenderTexture(ScreenWidth, ScreenHeight, TextureFormat::R32));

	mLightBuffer = shared_ptr<RenderTexture>(new RenderTexture(ScreenWidth, ScreenHeight, TextureFormat::R32G32B32A32));
	
	mGeometryBuffer = shared_ptr<GeometryBuffer>(new 
		GeometryBuffer(ScreenWidth,ScreenHeight,SCREEN_FAR,SCREEN_NEAR));

	mQuad = shared_ptr<Quad>(new Quad());

	mDebugWindow = shared_ptr<DebugWindow>(new DebugWindow(ScreenWidth,ScreenHeight,120,120));

	mBackDepthBufferRT = shared_ptr<DepthBufferRT>(new DepthBufferRT(ScreenWidth, ScreenHeight));

	mSSRBuffer = shared_ptr<SSRGBuffer>(new 
		SSRGBuffer(ScreenWidth, ScreenHeight, SCREEN_FAR, SCREEN_NEAR));

	ssaoManager = shared_ptr<SSAOManager>(new SSAOManager(ScreenWidth, ScreenHeight));
	skyBox = shared_ptr<SkyBox>(new SkyBox(L"Resource/Texture/sunsetcube.dds"));
	
	return true;
}


bool GraphicsSystem::Frame()
{
	bool result;
	float deltaTime;
	int mouseXOffset, mouseYOffset;
	static float rotateY = 0.0f;

	//进行InputClass对象的帧函数
	result = mInputClass->Frame();
	if (!result)
	{
	return false;
	}
	
	FPS* fpsPtr = FPS::GetInstance();
	fpsPtr->Frame();

	int fps = fpsPtr->GetFPS();

	deltaTime = fpsPtr->GetDeltaTime();
	float currentTime = fpsPtr->GetTime();
	mInputClass->GetMousePositionOffset(mouseXOffset, mouseYOffset);


	//鼠标右键处于按下的状态才能进行（左右移动）（前后移动）（旋转的操作）
	if (mInputClass->IsMouseRightButtuonPressed()&& fps >=5 && fps <=1000000)
	{
		//"W","S"键操作
		if (mInputClass->IsWPressed())
		{
			GCamera->Walk(deltaTime*CAMERA_SPEED);
		}
		else if (mInputClass->IsSPressed())
		{
			GCamera->Walk(-deltaTime*CAMERA_SPEED);
		}

		//"A","D"键操作
		if (mInputClass->IsAPressed())
		{
			GCamera->Strafe(-deltaTime*CAMERA_SPEED);
		}
		else if (mInputClass->IsDPressed())
		{
			GCamera->Strafe(deltaTime*CAMERA_SPEED);
		}

		//"Q","E"键操作
		if (mInputClass->IsQPressed())
		{
			GCamera->UpDown(-deltaTime*CAMERA_SPEED);
		}
		else if (mInputClass->IsEPressed())
		{
			GCamera->UpDown(deltaTime*CAMERA_SPEED);
		}

		//进行视角上下的旋转(跟刚开始的旋转角度在正负90度之间)
		if (rotateY <= 90.0f && rotateY >= -90.0f)
		{
			rotateY += (float)mouseYOffset*deltaTime;
			GCamera->Pitch((float)mouseYOffset*deltaTime*2.0f);
		}
	
		//进行视角左右的旋转
		GCamera->RotateY((float)mouseXOffset*deltaTime*2.0f);
	}

	//1与2切换渲染模式
	if (mInputClass->IsKeyDown(DIK_1))
	{
		GDirectxCore->RecoverDefualtRS();
	}
	if (mInputClass->IsKeyDown(DIK_2))
	{
		GDirectxCore->TurnOnWireFrameRender();
	}
	if (mInputClass->IsKeyDown(DIK_3))
	{
		materialType = MaterialType::DIFFUSE;
	}
	if (mInputClass->IsKeyDown(DIK_4))
	{
		materialType = MaterialType::DIFFUSE_NORMAL;
	}
	if (mInputClass->IsKeyDown(DIK_5))
	{
		materialType = MaterialType::DIFFUSE_SPECULAR;
	}
	if (mInputClass->IsKeyDown(DIK_6))
	{
		materialType = MaterialType::DIFFUSE_NORMAL_SPECULAR;
	}

	if (mInputClass->IsKeyDown(DIK_7))
	{
		bSSAO = false;
	}
	else
	{
		bSSAO = true;
	}

	GCamera->UpdateViewMatrix();
	//如果按下ESC，则破坏窗口
	if (mInputClass->IsEscapePressed())
	{
		DestroyWindow(mhwnd);
	}

	GCamera->UpdateViewMatrix();

	mCascadeShadowsManager->Update();

	return true;
}


void GraphicsSystem::Render()
{
	//**************************************************************************
	//绘制整个场景
	//*************************************************************************

	g_RenderMask->BeginEvent(L"BeginScene");
	GDirectxCore->BeginScene(0.3f, 0.0f, 1.0f, 1.0f);
	g_RenderMask->EndEvent();


	//绘制不透明物体
	g_RenderMask->BeginEvent(L"RenderOpacity");
	RenderOpacity();
	g_RenderMask->EndEvent();

	//获取整个场景的BackDepthBuffer
	#if SSR
	g_RenderMask->BeginEvent(L"RenderSceneBackDepthBuffer");
	RenderSceneBackDepthBuffer();
	g_RenderMask->EndEvent();
	#endif
	
	//绘制透明物体(普通的透明物体，SSR)
	g_RenderMask->BeginEvent(L"RenderTransparency");
	RenderTransparency();
	g_RenderMask->EndEvent();

	g_RenderMask->BeginEvent(L"RenderPostEffect");
	RenderPostEffectPass();
	g_RenderMask->EndEvent();

	g_RenderMask->BeginEvent(L"RenderSkyBox");
	RenderSkyBoxPass();
	g_RenderMask->EndEvent();

	#if DEBUG_GBUFFER
	g_RenderMask->BeginEvent(L"DEBUG_GBUFFER");
	RenderDebugWindow();
	g_RenderMask->EndEvent();
	#endif

	//**************************************************************************
	//结束绘制
	//*************************************************************************
	GDirectxCore->EndScene();
}

void GraphicsSystem::RenderGeometryPass()
{
	mGeometryBuffer->SetRenderTarget(XMFLOAT3(0.0f, 0.0f, 0.5f));

	vector<shared_ptr<GameObject>> vecGameObject = GGameObjectManager->m_vecGameObject;

	for (int index = 0; index < (int)vecGameObject.size(); index++)
	{
		shared_ptr<GameObject> pGameObject = vecGameObject[index];
		if (!pGameObject->m_pMesh->bTransparent)
		{
			if (!pGameObject->m_pMesh->bReflect)
			{
				pGameObject->Render();
			}
			else
			{
				//GDirectxCore->TurnOnMaskReflectDSS();
				pGameObject->Render();
				//GDirectxCore->TurnOnZBuffer();
			}
		
		}
	}
}


void GraphicsSystem::RenderLightingPass()
{
	RenderDirLightPass();
	RenderPointLightPass();
	RenderFinalShadingPass();
}


void GraphicsSystem::RenderPostEffectPass()
{
	g_RenderMask->BeginEvent(L"FXAA");
	GDirectxCore->SetBackBufferRender();
	GDirectxCore->SetViewPort();
	GDirectxCore->TurnOffZBuffer();
	GShaderManager->fxaaShader->SetTexture("inputTexture", mSrcRT->GetSRV());
	GShaderManager->fxaaShader->SetFloat2("rcpFrame", XMFLOAT2(1.0f / (float)m_nScreenWidth, 1.0f / (float)m_nScreenHeight));
	GShaderManager->fxaaShader->SetTextureSampler("anisotropicSampler", GTextureSamplerManager->GetTextureSampler(TextureSampler::Anisotropic));
	GShaderManager->fxaaShader->Apply();
	mQuad->Render();
	GDirectxCore->TurnOnZBuffer();
	g_RenderMask->EndEvent();
}

void GraphicsSystem::RenderDebugWindow()
{
	GDirectxCore->SetBackBufferRender();
	GDirectxCore->SetViewPort();
	GDirectxCore->TurnOffZBuffer();

	GShaderManager->uiShader->SetMatrix("UIView", GCamera->GetUIViewMatrix());
	GShaderManager->uiShader->SetMatrix("UIOrtho", GCamera->GetUIOrthoMatrix());

	//diffuse
	GShaderManager->uiShader->SetTexture("ShaderTexture", 
		mGeometryBuffer->GetGBufferSRV(GBufferType::Diffuse));
	GShaderManager->uiShader->Apply();
	mDebugWindow->Render(10, 600);

	//pos
	GShaderManager->uiShader->SetTexture("ShaderTexture",
		mGeometryBuffer->GetGBufferSRV(GBufferType::Pos));
	GShaderManager->uiShader->Apply();
	mDebugWindow->Render(130, 600);

	//normal
	GShaderManager->uiShader->SetTexture("ShaderTexture",
		mGeometryBuffer->GetGBufferSRV(GBufferType::Normal));
	GShaderManager->uiShader->Apply();
	mDebugWindow->Render(250, 600);

	//specular
	GShaderManager->uiShader->SetTexture("ShaderTexture",
		mGeometryBuffer->GetGBufferSRV(GBufferType::SpecularRoughMetal));
	GShaderManager->uiShader->Apply();
	mDebugWindow->Render(370, 600);


	//ShadowMap
	GShaderManager->uiShader->SetTexture("ShaderTexture",
		mGrayShadowMap->GetSRV());
	GShaderManager->uiShader->Apply();
	mDebugWindow->Render(610, 600);

	//SSAO
	GShaderManager->uiShader->SetTexture("ShaderTexture",
		ssaoManager->GetSsaoSRV());
	GShaderManager->uiShader->Apply();
	mDebugWindow->Render(730, 600);

	//Depth
	GShaderManager->depthDisplayShader->SetTexture("ShaderTexture",
		mGeometryBuffer->GetGBufferSRV(GBufferType::Depth));
	GShaderManager->depthDisplayShader->SetMatrix("UIView", GCamera->GetUIViewMatrix());
	GShaderManager->depthDisplayShader->SetMatrix("UIOrtho", GCamera->GetUIOrthoMatrix());
	GShaderManager->depthDisplayShader->SetFloat("farPlane", GCamera->mFarPlane);
	GShaderManager->depthDisplayShader->SetFloat("nearPlane", GCamera->mNearPlane);
	GShaderManager->depthDisplayShader->Apply();
	mDebugWindow->Render(490, 600);

	#if SSR
	GShaderManager->SetDepthShader
	(mBackDepthBufferRT->GetShaderResourceView());
	mDebugWindow->Render(610, 600);

	GShaderManager->SetUIShader
	(mSSRBuffer->GetGBufferSRV(SSRBufferType::VIEW_NORMAL));
	mDebugWindow->Render(10, 480);

	GShaderManager->SetUIShader
	(mSSRBuffer->GetGBufferSRV(SSRBufferType::VIEW_POS));
	mDebugWindow->Render(10, 360);
	#endif
	
	GDirectxCore->TurnOnZBuffer();
}

void GraphicsSystem::RenderSSRPass()
{
	//mSSRRT->SetRenderTarget();
	ID3D11RenderTargetView* backRTV =GDirectxCore->GetRTV();
	//靠模板緩存值來判斷
	g_pDeviceContext->OMSetRenderTargets(1, &backRTV, nullptr);
	GDirectxCore->SetViewPort();
	GDirectxCore->TurnOnAlphaBlend();
	GDirectxCore->TurnOffZBuffer();
	XMMATRIX worldMatrix = mSponzaBottom->GetWorldMatrix();
	XMMATRIX projMatrix = GCamera->GetProjectionMatrix();
	XMFLOAT4X4 projFloat4X4;
	XMStoreFloat4x4(&projFloat4X4, projMatrix);
	XMFLOAT2 perspectiveValues;
	perspectiveValues.x = projFloat4X4.m[3][2];
	perspectiveValues.y = -projFloat4X4.m[2][2];
	GShaderManager->ssrShader->SetMatrix("View", GCamera->GetViewMatrix());
	GShaderManager->ssrShader->SetMatrix("Proj", GCamera->GetProjectionMatrix());
	GShaderManager->ssrShader->SetFloat("farPlane", GCamera->mFarPlane);
	GShaderManager->ssrShader->SetFloat("nearPlane", GCamera->mNearPlane);
	GShaderManager->ssrShader->SetFloat2("perspectiveValues", perspectiveValues);
	GShaderManager->ssrShader->SetTexture("DiffuseTex", mSrcRT->GetSRV());
	GShaderManager->ssrShader->SetTexture("FrontDepthTex", mGeometryBuffer->GetGBufferSRV(GBufferType::Depth));
	GShaderManager->ssrShader->SetTexture("BackDepthTex", mBackDepthBufferRT->GetShaderResourceView());
	GShaderManager->ssrShader->SetTexture("ViewPosTex", mSSRBuffer->GetGBufferSRV(SSRBufferType::VIEW_POS));
	GShaderManager->ssrShader->SetTexture("ViewNormalTex", mSSRBuffer->GetGBufferSRV(SSRBufferType::VIEW_NORMAL));
	GShaderManager->ssrShader->Apply();
	mQuad->Render();
	GDirectxCore->TurnOnZBuffer();
	GDirectxCore->TurnOffAlphaBlend();
}

void GraphicsSystem::RenderOpacity()
{
	//g_RenderMask->BeginEvent(L"RenderGeometryPass");
	RenderGeometryPass();
	//g_RenderMask->EndEvent();

	g_RenderMask->BeginEvent(L"RenderShadowMapPass");
	RenderShadowMapPass();
	g_RenderMask->EndEvent();

	g_RenderMask->BeginEvent(L"RenderSSAOPass");
	RenderSSAOPass();
	g_RenderMask->EndEvent();

	g_RenderMask->BeginEvent(L"RenderLightingPass");
	RenderLightingPass();
	g_RenderMask->EndEvent();
}

//绘制透明物体分为绘制透明
void GraphicsSystem::RenderTransparency()
{
	RenderGeneralTransparency();

	#if SSR
	RenderSSR();
	#endif
}

void GraphicsSystem::RenderGeneralTransparency()
{
	ID3D11RenderTargetView* backRTV = GDirectxCore->GetRTV();
	ID3D11DepthStencilView* opacityDSV = mGeometryBuffer->GetDSV();
	g_pDeviceContext->OMSetRenderTargets(1, &backRTV, opacityDSV);
	GDirectxCore->SetViewPort();
	GDirectxCore->TurnOnDisbleZWriteDSS();
	GDirectxCore->TurnOnAlphaBlend();

	vector<shared_ptr<GameObject>> vecGameObject = GGameObjectManager->m_vecGameObject;

	for (int index = 0; index < (int)vecGameObject.size(); index++)
	{
		shared_ptr<GameObject> pGameObject = vecGameObject[index];
		if (pGameObject->m_pMesh->bTransparent && !pGameObject->m_pMesh->bReflect)
		{	
			/*XMMATRIX worldMatrix = pGameObject->GetWorldMatrix();
			//GShaderManager->(worldMatrix, XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f));
			GShaderManager->mShader->SetMatrix("World", worldMatrix);
			GShaderManager->mShader->SetMatrix("View", GCamera->GetViewMatrix());
			GShaderManager->mShader->SetMatrix("Proj", GCamera->GetProjectionMatrix());
			GShaderManager->mShader->Apply();
			pGameObject->RenderMesh();*/
		}
	}

	GDirectxCore->TurnOffAlphaBlend();
	GDirectxCore->RecoverDefaultDSS();
}


void GraphicsSystem::RenderSceneBackDepthBuffer()
{
	mBackDepthBufferRT->SetRenderTarget();
	GDirectxCore->TurnOnCullFront();
	vector<shared_ptr<GameObject>> vecGameObject = GGameObjectManager->m_vecGameObject;

	for (int index = 0; index < (int)vecGameObject.size(); index++)
	{
		shared_ptr<GameObject> pGameObject = vecGameObject[index];
		if (!pGameObject->m_pMesh->bTransparent)
		{
			pGameObject->Render();
		}
	}

	//恢复默认的RS
	GDirectxCore->RecoverDefualtRS();

}

void GraphicsSystem::RenderSSRBufferPass()
{
	ID3D11DepthStencilView* backDSV = mGeometryBuffer->GetDSV();
	mSSRBuffer->SetRenderTarget(backDSV);
	GDirectxCore->TurnOnEnableReflectDSS();
	GShaderManager->ssrGBufferShader->SetTexture("WorldPosTex", mGeometryBuffer->GetGBufferSRV(GBufferType::Pos));
	GShaderManager->ssrGBufferShader->SetTexture("WorldNormalTex", mGeometryBuffer->GetGBufferSRV(GBufferType::Normal));
	GShaderManager->ssrGBufferShader->SetMatrix("View", GCamera->GetViewMatrix());
	GShaderManager->ssrGBufferShader->Apply();
	mQuad->Render();
	GDirectxCore->RecoverDefaultDSS();
	GDirectxCore->SetBackBufferRender();

}

void  GraphicsSystem::RenderSSR()
{
	RenderSSRBufferPass();
	RenderSSRPass();
}


void GraphicsSystem::InitDebugConsole()
{
	AllocConsole();
	FILE* pf;
	freopen_s(&pf, "CONOUT$", "w", stdout);   // 重定向输出
}


void GraphicsSystem::CloseDebugConsole()
{
	FreeConsole();
}


void GraphicsSystem::RenderPointLightPass()
{
	ID3D11RenderTargetView* backRTV[1] = { nullptr };
	ID3D11DepthStencilView* opacityDSV = mGeometryBuffer->GetDSV(); 
	ID3D11RenderTargetView* pLightRTV = mLightBuffer->GetRenderTargetView();
	
	ID3D11ShaderResourceView* shaderResourceView[4];
	shaderResourceView[0] = mGeometryBuffer->GetGBufferSRV(GBufferType::Pos);
	shaderResourceView[1] = mGeometryBuffer->GetGBufferSRV(GBufferType::Normal);
	shaderResourceView[2] = mGeometryBuffer->GetGBufferSRV(GBufferType::SpecularRoughMetal);
	shaderResourceView[3] = mGeometryBuffer->GetGBufferSRV(GBufferType::Diffuse);

	for (int index = 0; index < (int)GLightManager->m_vecPointLight.size(); ++index)
	{
		g_pDeviceContext->OMSetRenderTargets(1, backRTV, opacityDSV);
		GDirectxCore->TurnOnMaskLightVolumeDSS();
		GDirectxCore->TurnOffFaceCull();
		shared_ptr<PointLight> pPoinntLight = GLightManager->m_vecPointLight[index];
		GShaderManager->forwardPureColorShader->SetMatrix("World", pPoinntLight->GetWorldMatrix());
		GShaderManager->forwardPureColorShader->SetMatrix("View", GCamera->GetViewMatrix());
		GShaderManager->forwardPureColorShader->SetMatrix("Proj", GCamera->GetProjectionMatrix());
		GShaderManager->forwardPureColorShader->SetMatrix("WorldInvTranspose", FMath::GetInvenseTranspose(pPoinntLight->GetWorldMatrix()));
		GShaderManager->forwardPureColorShader->Apply();
		m_pPointVolume->RenderMesh();

		g_pDeviceContext->OMSetRenderTargets(1, &pLightRTV, opacityDSV);
		GDirectxCore->TurnOnLightBlend();
		GDirectxCore->TurnOnRenderLightVolumeDSS();
		shared_ptr<PointLight> pPointLight = GLightManager->m_vecPointLight[index];
		XMFLOAT3 lightCol = pPointLight->GetLightColor();
		XMFLOAT4 lightColor = XMFLOAT4(lightCol.x, lightCol.y, lightCol.z, pPointLight->GetLightIntensity());
		GShaderManager->defferedPointLightShader->SetTexture("WorldPosTex", shaderResourceView[0]);
		GShaderManager->defferedPointLightShader->SetTexture("WorldNormalTex", shaderResourceView[1]);
		GShaderManager->defferedPointLightShader->SetTexture("SpecularRoughMetalTex", shaderResourceView[2]);
		GShaderManager->defferedPointLightShader->SetTexture("AlbedoTex", shaderResourceView[3]);
		GShaderManager->defferedPointLightShader->SetMatrix("World", pPointLight->GetWorldMatrix());
		GShaderManager->defferedPointLightShader->SetMatrix("View", GCamera->GetViewMatrix());
		GShaderManager->defferedPointLightShader->SetMatrix("Proj", GCamera->GetProjectionMatrix());
		GShaderManager->defferedPointLightShader->SetFloat3("cameraPos", GCamera->GetPosition());
		GShaderManager->defferedPointLightShader->SetFloat4("lightColor", lightColor);
		GShaderManager->defferedPointLightShader->SetFloat3("lightPos", pPointLight->GetPosition());
		GShaderManager->defferedPointLightShader->SetFloat("radius", pPointLight->GetRadius());
		GShaderManager->defferedPointLightShader->SetFloat4("attenuation", pPointLight->GetLightAttenuation());
		GShaderManager->defferedPointLightShader->SetTextureSampler("clampLinearSample", GTextureSamplerBilinearClamp);
		GShaderManager->defferedPointLightShader->Apply();
		m_pPointVolume->RenderMesh();
		GDirectxCore->TurnOffAlphaBlend();
	}

	GDirectxCore->RecoverDefualtRS();

}


void GraphicsSystem::RenderDirLightPass()
{
	mLightBuffer->SetRenderTarget();
	ID3D11ShaderResourceView* shaderResourceView[6];
	shaderResourceView[0] = mGeometryBuffer->GetGBufferSRV(GBufferType::Pos);
	shaderResourceView[1] = mGeometryBuffer->GetGBufferSRV(GBufferType::Normal);
	shaderResourceView[2] = mGeometryBuffer->GetGBufferSRV(GBufferType::SpecularRoughMetal);
	shaderResourceView[3] = mGrayShadowMap->GetSRV();
	shaderResourceView[4] = ssaoManager->GetSsaoSRV();
	shaderResourceView[5] = mGeometryBuffer->GetGBufferSRV(GBufferType::Diffuse);

	GDirectxCore->TurnOffZBuffer();
	GDirectxCore->TurnOnLightBlend();

	for (int index = 0; index < (int)GLightManager->m_vecDirLight.size(); ++index)
	{
		shared_ptr<DirectionLight> pDirLight = GLightManager->m_vecDirLight[index];
		XMFLOAT3 lightCo = pDirLight->GetLightColor();
		XMFLOAT4 lightColor = XMFLOAT4(lightCo.x, lightCo.y, lightCo.z, pDirLight->GetLightIntensity());
		GShaderManager->defferedDirLightShader->SetTexture("WorldPosTex", shaderResourceView[0]);
		GShaderManager->defferedDirLightShader->SetTexture("WorldNormalTex", shaderResourceView[1]);
		GShaderManager->defferedDirLightShader->SetTexture("SpecularRoughMetalTex", shaderResourceView[2]);
		GShaderManager->defferedDirLightShader->SetTexture("DirLightShadowMap", shaderResourceView[3]);
		GShaderManager->defferedDirLightShader->SetTexture("SSAORT", shaderResourceView[4]);
		GShaderManager->defferedDirLightShader->SetTexture("AlbedoTex", shaderResourceView[5]);
		GShaderManager->defferedDirLightShader->SetFloat3("cameraPos", GCamera->GetPosition());
		GShaderManager->defferedDirLightShader->SetFloat4("lightColor", lightColor);
		GShaderManager->defferedDirLightShader->SetFloat3("lightDir", pDirLight->GetLightDirection());
		GShaderManager->defferedDirLightShader->SetTextureSampler("clampLinearSample", GTextureSamplerBilinearClamp);
		
		ID3D11ShaderResourceView* memSSAORT = nullptr;
		if (bSSAO)
		{
			memSSAORT = ssaoManager->GetSsaoSRV();
		}
		else
		{
			memSSAORT = GWhiteTexture;
		}
		GShaderManager->defferedDirLightShader->SetTexture("SSAORT", memSSAORT);
		GShaderManager->defferedDirLightShader->Apply();
		mQuad->Render();
	}

	GDirectxCore->TurnOffAlphaBlend();
	GDirectxCore->RecoverDefaultDSS();
}

void GraphicsSystem::RenderFinalShadingPass()
{
	ID3D11RenderTargetView* pSceneRTV = mSrcRT->GetRenderTargetView();
	ID3D11DepthStencilView* opacityDSV = mGeometryBuffer->GetDSV();
	GDirectxCore->TurnOffZBuffer();
	GDirectxCore->SetViewPort();
	g_pDeviceContext->OMSetRenderTargets(1, &pSceneRTV, opacityDSV);
	GShaderManager->defferedFinalShader->SetTexture("DiffuseTex", mLightBuffer->GetSRV());
	GShaderManager->defferedFinalShader->SetTextureSampler("clampLinearSample", GTextureSamplerBilinearClamp);
	GShaderManager->defferedFinalShader->Apply();
	mQuad->Render();
	
	GDirectxCore->RecoverDefaultDSS();
}

void GraphicsSystem::RenderShadowMapPass()
{
	if (GLightManager->m_vecDirLight.size() <= 0)
		return;

	XMMATRIX lightViewMatrix = GLightManager->GetMainLight()->GetViewMatrix();

	//渲染需要投射阴影的物体到RT上
	//后面可以考虑用GeometryShader减少DrawCall
	mCascadeShadowsManager->ClearDepthBuffer();
	for (int nCascadeIndex = 0; nCascadeIndex < CASCADE_SHADOW_MAP_NUM; ++nCascadeIndex)
	{
		mCascadeShadowsManager->SetRenderTarget(nCascadeIndex);

		for (int index = 0; index < (int)GGameObjectManager->m_vecGameObject.size(); ++index)
		{
			shared_ptr<GameObject> memGo = GGameObjectManager->m_vecGameObject[index];
			if (memGo->m_pMesh && !memGo->m_pMesh->bTransparent)
			{
				if (memGo->m_pMesh->bCastShadow)
				{
					GShaderManager->lightDepthShader->SetMatrix("World", memGo->GetWorldMatrix());
					GShaderManager->lightDepthShader->SetMatrix("View", lightViewMatrix);
					GShaderManager->lightDepthShader->SetMatrix("Proj", mCascadeShadowsManager->mArrayLightOrthoMatrix[nCascadeIndex]);
					GShaderManager->lightDepthShader->Apply();
					memGo->RenderMesh();
				}
			}
		}
	}

	//渲染得到阴影
	mGrayShadowMap->SetRenderTarget();
	GDirectxCore->TurnOffZBuffer();
	GShaderManager->shadowMapShader->SetTexture("WorldPosTex", mGeometryBuffer->GetGBufferSRV(GBufferType::Pos));
	GShaderManager->shadowMapShader->SetTexture("CascadeLightDepthMap", mCascadeShadowsManager->GetShadowMapSRV());
	GShaderManager->shadowMapShader->SetMatrix("dirView", lightViewMatrix);
	for (int nCascadeIndex = 0; nCascadeIndex < CASCADE_SHADOW_MAP_NUM; ++nCascadeIndex)
	{
		GShaderManager->shadowMapShader->SetMatrixArrayElement("arrayDirProj", 
			mCascadeShadowsManager->mArrayLightOrthoMatrix[nCascadeIndex], nCascadeIndex);
	}

	GShaderManager->shadowMapShader->SetFloat("shadowBias", 0.003f);
	GShaderManager->shadowMapShader->Apply();
	mQuad->Render();

	GDirectxCore->RecoverDefaultDSS();
}

void GraphicsSystem::RenderSSAOPass()
{
	//渲染得到SSAORT
	ssaoManager->Render(mGeometryBuffer.get());
}

void GraphicsSystem::RenderSkyBoxPass()
{
	skyBox->Render(mGeometryBuffer.get());
}