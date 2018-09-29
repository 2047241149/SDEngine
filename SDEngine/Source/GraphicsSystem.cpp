#include "GraphicsSystem.h"

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

	//����D3DClass�ಢ�ҳ�ʼ��,D3DClass�ǵ�һ�����������ҳ�ʼ��
	GDirectxCore->Init(ScreenWidth, ScreenHeight, VSYNC_ENABLE, hwnd, FULL_SCREEN, SCREEN_NEAR, SCREEN_FAR);

	//Console��Debug����
	InitDebugConsole();

	//�������
	GCamera->SetProjParams(XM_PI / 3.0f, (float)ScreenWidth / (float)ScreenHeight, SCREEN_NEAR, SCREEN_FAR);
	GCamera->SetUIOrthoParams((float)ScreenWidth, (float)ScreenHeight);

	//����Ϸ��ӵƹ� 
	shared_ptr<DirectionLight> m_spDirLight = shared_ptr<DirectionLight>(new DirectionLight());
	m_spDirLight->SetLightDiretion(XMFLOAT3(-0.5f, -1.0f, 0.0f));
	m_spDirLight->SetAmbientLight(XMFLOAT3(0.1f, 0.1f, 0.1f));
	m_spDirLight->SetLightColor(XMFLOAT3(1.0f, 1.0f, 1.0f));
	m_spDirLight->SetLightPostion(XMFLOAT3(10.0f, 10.0f, 10.0f));
	GLightManager->Add(m_spDirLight);

	shared_ptr<PointLight> m_PointLight = shared_ptr<PointLight>(new PointLight());
	m_PointLight->SetLightColor(XMFLOAT3(1.0f, 1.0f, 1.0f));
	m_PointLight->SetLightIntensity(40.0f);
	m_PointLight->SetRadius(30.0f);
	m_PointLight->SetLightPostion(XMFLOAT3(20.0f, 1.0f, -6.0f));

	shared_ptr<PointLight> m_PointLight1 = shared_ptr<PointLight>(new PointLight());
	m_PointLight1->SetLightColor(XMFLOAT3(1.0f, 0.0f, 0.0f));
	m_PointLight1->SetLightIntensity(40.0f);
	m_PointLight1->SetRadius(30.0f);
	m_PointLight1->SetLightPostion(XMFLOAT3(20.0f, 1.0f, 5.0f));

	GLightManager->Add(m_PointLight);
	GLightManager->Add(m_PointLight1);
	
	//����mesh
	//(1)
	shared_ptr<Mesh> pHeadMesh = shared_ptr<Mesh>(new Mesh("Resource\\FBXModel\\head\\head.FBX"));
	pHeadMesh->m_eMaterialType = MaterialType::DIFFUSE;
	pHeadMesh->bCastShadow = true;

	shared_ptr<Mesh> pOpacitySphereMesh = shared_ptr<Mesh>(new Mesh("Resource\\FBXModel\\sphere\\sphere.FBX"));
	pOpacitySphereMesh->m_eMaterialType = MaterialType::PURE_COLOR;
	pOpacitySphereMesh->pureColor = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	pOpacitySphereMesh->bCastShadow = true;

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

	//����GameObject
	mHeadObject = shared_ptr<GameObject>(new GameObject());
	mHeadObject->SetMesh(pHeadMesh);
	mHeadObject->m_pTransform->localPosition = XMFLOAT3(0.0f, 5.0f, 0.0f);
	mHeadObject->m_pTransform->localRotation = XMFLOAT3(0.0f, 90.0f, 0.0f);
	mHeadObject->m_pTransform->localScale = XMFLOAT3(5.0f, 5.0f, 5.0f);

	mOpacitySphereObject = shared_ptr<GameObject>(new GameObject());
	mOpacitySphereObject->SetMesh(pOpacitySphereMesh);
	mOpacitySphereObject->m_pTransform->localScale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	mOpacitySphereObject->m_pTransform->localPosition = XMFLOAT3(0.0f, 2.0f, 0.0f);

	mSponzaBottom = shared_ptr<GameObject>(new GameObject());
	mSponzaBottom->SetMesh(pSponzaBottom);
	mSponzaBottom->m_pTransform->localPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);

	mSponzaNoBottom = shared_ptr<GameObject>(new GameObject());
	mSponzaNoBottom->SetMesh(pSponzaNoBottom);
	mSponzaNoBottom->m_pTransform->localPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);

	mTransSphereObject = shared_ptr<GameObject>(new GameObject());
	mTransSphereObject->SetMesh(pTransparentSphereMesh);
	mTransSphereObject->m_pTransform->localPosition = XMFLOAT3(10.0f, 10.0f, 0.0f);

	m_pPointVolume = shared_ptr<GameObject>(new GameObject());
	m_pPointVolume->SetMesh(pPointLightVolume);

	GGameObjectManager->Add(mHeadObject);
	GGameObjectManager->Add(mOpacitySphereObject);
	GGameObjectManager->Add(mSponzaBottom);
	GGameObjectManager->Add(mSponzaNoBottom);
	GGameObjectManager->Add(mTransSphereObject);

	//����������
	mInputClass = shared_ptr<Input>(new Input(hinstance, hwnd, ScreenWidth, ScreenHeight));

	mSrcRT = shared_ptr<RenderTexture>(
		new RenderTexture(ScreenWidth, ScreenHeight));

	mSSRRT = shared_ptr<RenderTexture>(
		new RenderTexture(ScreenWidth, ScreenHeight));

	mCascadeShadowsManager = shared_ptr<CascadedShadowsManager>(new CascadedShadowsManager(SHADOW_MAP_SIZE));

	mGrayShadowMap = shared_ptr<RenderTexture>(new RenderTexture(ScreenWidth, ScreenHeight));

	mLightBuffer = shared_ptr<RenderTexture>(new RenderTexture(ScreenWidth, ScreenHeight));

	mGeometryBuffer = shared_ptr<GeometryBuffer>(new 
		GeometryBuffer(ScreenWidth,ScreenHeight,SCREEN_FAR,SCREEN_NEAR));

	mQuad = shared_ptr<Quad>(new Quad());

	mDebugWindow = shared_ptr<DebugWindow>(new DebugWindow(ScreenWidth,ScreenHeight,120,120));

	mBackDepthBufferRT = shared_ptr<DepthBufferRT>(new DepthBufferRT(ScreenWidth, ScreenHeight));

	mSSRBuffer = shared_ptr<SSRGBuffer>(new 
		SSRGBuffer(ScreenWidth, ScreenHeight, SCREEN_FAR, SCREEN_NEAR));

	
	return true;
}


bool GraphicsSystem::Frame()
{
	bool result;
	float deltaTime;
	int mouseXOffset, mouseYOffset;
	static float rotateY = 0.0f;

	//����InputClass�����֡����
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


	//����Ҽ����ڰ��µ�״̬���ܽ��У������ƶ�����ǰ���ƶ�������ת�Ĳ�����
	if (mInputClass->IsMouseRightButtuonPressed()&& fps >=5 && fps <=1000000)
	{
		//"W","S"������
		if (mInputClass->IsWPressed())
		{
			GCamera->Walk(deltaTime*CAMERA_SPEED);
		}
		else if (mInputClass->IsSPressed())
		{
			GCamera->Walk(-deltaTime*CAMERA_SPEED);
		}

		//"A","D"������
		if (mInputClass->IsAPressed())
		{
			GCamera->Strafe(-deltaTime*CAMERA_SPEED);
		}
		else if (mInputClass->IsDPressed())
		{
			GCamera->Strafe(deltaTime*CAMERA_SPEED);
		}

		//"Q","E"������
		if (mInputClass->IsQPressed())
		{
			GCamera->UpDown(-deltaTime*CAMERA_SPEED);
		}
		else if (mInputClass->IsEPressed())
		{
			GCamera->UpDown(deltaTime*CAMERA_SPEED);
		}


		//�����ӽ����µ���ת(���տ�ʼ����ת�Ƕ�������90��֮��)
		if (rotateY <= 90.0f&&rotateY >= -90.0f)
		{
			rotateY += (float)mouseYOffset*deltaTime;
			GCamera->Pitch((float)mouseYOffset*deltaTime*2.0f);
		}
	
		//�����ӽ����ҵ���ת
		GCamera->RotateY((float)mouseXOffset*deltaTime*2.0f);
	}


	//1��2�л���Ⱦģʽ
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

	GCamera->UpdateViewMatrix();
	//�������ESC�����ƻ�����
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
	//������������
	//*************************************************************************

	GDirectxCore->BeginScene(0.3f, 0.0f, 1.0f, 1.0f);

	//���Ʋ�͸������
	RenderOpacity();

	//��ȡ����������BackDepthBuffer
	#if defined(SSR)
	RenderSceneBackDepthBuffer();
	#endif
	

	//����͸������(��ͨ��͸�����壬SSR)
	//RenderTransparency();

	/*#if defined(POST_EFFECT)
		RenderPostEffectPass();
	#endif // POST_EFFECT*/

	RenderShadowMapPass();

	#if defined(DEBUG_GBUFFER)
		RenderDebugWindow();
	#endif

	//**************************************************************************
	//��������
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

	GDirectxCore->SetBackBufferRender();
	GDirectxCore->SetViewPort();
	GDirectxCore->TurnOffZBuffer();
	GShaderManager->SetFXAAShader(mSrcRT->GetSRV(), (float)m_nScreenWidth, (float)m_nScreenHeight);
	mQuad->Render();

	GDirectxCore->TurnOnZBuffer();
}


void GraphicsSystem::RenderPostEffectPass()
{

}

void GraphicsSystem::RenderDebugWindow()
{
	GDirectxCore->SetBackBufferRender();
	GDirectxCore->SetViewPort();
	GDirectxCore->TurnOffZBuffer();

	//diffuse
	GShaderManager->SetUIShader
	(mGeometryBuffer->GetGBufferSRV(GBufferType::Diffuse));
	mDebugWindow->Render(10, 600);

	//pos
	GShaderManager->SetUIShader
	(mGeometryBuffer->GetGBufferSRV(GBufferType::Pos));
	mDebugWindow->Render(130, 600);

	//normal
	GShaderManager->SetUIShader
	(mGeometryBuffer->GetGBufferSRV(GBufferType::Normal));
	mDebugWindow->Render(250, 600);

	//specular
	GShaderManager->SetUIShader
	(mGeometryBuffer->GetGBufferSRV(GBufferType::Specular));
	mDebugWindow->Render(370, 600);

	GShaderManager->SetDepthShader
	(mGeometryBuffer->GetGBufferSRV(GBufferType::Depth));
	mDebugWindow->Render(490, 600);

	GShaderManager->SetUIShader(mGrayShadowMap->GetSRV());
	mDebugWindow->Render(610, 600);

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
	//��ģ�徏��ֵ���Д�
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
	ID3D11ShaderResourceView* arraySRV[5];
	arraySRV[0] = mSrcRT->GetSRV();
	arraySRV[1] = mGeometryBuffer->GetGBufferSRV(GBufferType::Depth);
	arraySRV[2] = mBackDepthBufferRT->GetShaderResourceView();
	arraySRV[3] = mSSRBuffer->GetGBufferSRV(SSRBufferType::VIEW_POS);
	arraySRV[4] = mSSRBuffer->GetGBufferSRV(SSRBufferType::VIEW_NORMAL);
	GShaderManager->SetSSRShader(worldMatrix,arraySRV,perspectiveValues);
	mQuad->Render();
	GDirectxCore->TurnOnZBuffer();
	GDirectxCore->TurnOffAlphaBlend();
}

void GraphicsSystem::RenderOpacity()
{
	RenderGeometryPass();
	RenderShadowMapPass();
	RenderLightingPass();
}

//����͸�������Ϊ����͸��
void GraphicsSystem::RenderTransparency()
{
	RenderGeneralTransparency();

	#if defined(SSR)
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
			XMMATRIX worldMatrix = pGameObject->GetWorldMatrix();
			GShaderManager->SetForwardPureColorShader(worldMatrix, XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f));
			pGameObject->RenderMesh();
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

	//�ָ�Ĭ�ϵ�RS
	GDirectxCore->RecoverDefualtRS();

}

void GraphicsSystem::RenderSSRBufferPass()
{
	ID3D11DepthStencilView* backDSV = mGeometryBuffer->GetDSV();
	mSSRBuffer->SetRenderTarget(backDSV);
	GDirectxCore->TurnOnEnableReflectDSS();
	ID3D11ShaderResourceView* arraySRV[2];
	arraySRV[0] = mGeometryBuffer->GetGBufferSRV(GBufferType::Pos);
	arraySRV[1] = mGeometryBuffer->GetGBufferSRV(GBufferType::Normal);
	GShaderManager->SetSSRGBufferShader(arraySRV);
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
	freopen_s(&pf, "CONOUT$", "w", stdout);   // �ض������
}


void GraphicsSystem::CloseDebugConsole()
{
	FreeConsole();
}


void GraphicsSystem::RenderPointLightPass()
{

	ID3D11RenderTargetView* backRTV[1] = { nullptr };
	ID3D11DepthStencilView* opacityDSV = mGeometryBuffer->GetDSV();
	ID3D11RenderTargetView* pSceneRTV = mSrcRT->GetRenderTargetView();
	ID3D11RenderTargetView* pLightRTV = mLightBuffer->GetRenderTargetView();
	
	ID3D11ShaderResourceView* shaderResourceView[3];
	shaderResourceView[0] = mGeometryBuffer->GetGBufferSRV(GBufferType::Pos);
	shaderResourceView[1] = mGeometryBuffer->GetGBufferSRV(GBufferType::Normal);
	shaderResourceView[2] = mGeometryBuffer->GetGBufferSRV(GBufferType::Specular);

	for (int index = 0; index < (int)GLightManager->m_vecPointLight.size(); ++index)
	{
		g_pDeviceContext->OMSetRenderTargets(1, backRTV, opacityDSV);
		GDirectxCore->TurnOnMaskLightVolumeDSS();
		GDirectxCore->TurnOffFaceCull();
		shared_ptr<PointLight> pPoinntLight = GLightManager->m_vecPointLight[index];
		GShaderManager->SetForwardPureColorShader(pPoinntLight->GetWorldMatrix(), XMVectorSet(1.0f,1.0f,1.0,1.0f));
		m_pPointVolume->RenderMesh();

		g_pDeviceContext->OMSetRenderTargets(1, &pLightRTV, opacityDSV);
		GDirectxCore->TurnOnLightBlend();
		GDirectxCore->TurnOnRenderLightVolumeDSS();
		GShaderManager->SetDefferedPointLightShader(shaderResourceView, index);
		m_pPointVolume->RenderMesh();
		GDirectxCore->TurnOffAlphaBlend();
	}

	GDirectxCore->RecoverDefualtRS();

}


void GraphicsSystem::RenderDirLightPass()
{

	mLightBuffer->SetRenderTarget();
	ID3D11ShaderResourceView* shaderResourceView[4];
	shaderResourceView[0] = mGeometryBuffer->GetGBufferSRV(GBufferType::Pos);
	shaderResourceView[1] = mGeometryBuffer->GetGBufferSRV(GBufferType::Normal);
	shaderResourceView[2] = mGeometryBuffer->GetGBufferSRV(GBufferType::Specular);
	shaderResourceView[3] = mGrayShadowMap->GetSRV();

	GDirectxCore->TurnOffZBuffer();
	GDirectxCore->TurnOnLightBlend();

	for (int index = 0; index < (int)GLightManager->m_vecDirLight.size(); ++index)
	{
		GShaderManager->SetDefferedDirLightShader(shaderResourceView, index);
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
	ID3D11ShaderResourceView* pShaderViewArray[2];
	pShaderViewArray[0] = mGeometryBuffer->GetGBufferSRV(GBufferType::Diffuse);
	pShaderViewArray[1] = mLightBuffer->GetSRV();
	GShaderManager->SetDefferedFinalShader(pShaderViewArray);
	mQuad->Render();
	
	GDirectxCore->RecoverDefaultDSS();
}

void GraphicsSystem::RenderShadowMapPass()
{
		
	//��Ⱦ��ҪͶ����Ӱ�����嵽RT��
	//������Կ�����GeometryShader����DrawCall
	mCascadeShadowsManager->ClearDepthBuffer();
	for (int nCascadeIndex = 0; nCascadeIndex < CASCADE_SHADOW_MAP_NUM; ++nCascadeIndex)
	{
		mCascadeShadowsManager->SetRenderTarget(nCascadeIndex);

		for (int index = 0; index < GGameObjectManager->m_vecGameObject.size(); ++index)
		{
			shared_ptr<GameObject> memGo = GGameObjectManager->m_vecGameObject[index];
			if (memGo->m_pMesh && !memGo->m_pMesh->bTransparent)
			{
				if (memGo->m_pMesh->bCastShadow)
				{
					GShaderManager->SetLightDepthShader(memGo->GetWorldMatrix(), mCascadeShadowsManager->mArrayLightOrthoMatrix[nCascadeIndex]);
					memGo->RenderMesh();
				}
			}
		}
	}

	//��Ⱦ�õ���Ӱ
	mGrayShadowMap->SetRenderTarget();
	GDirectxCore->TurnOffZBuffer();
	GShaderManager->SetShadowMapShader(mGeometryBuffer->GetGBufferSRV(GBufferType::Pos), mCascadeShadowsManager.get());
	mQuad->Render();
	GDirectxCore->RecoverDefaultDSS();
}