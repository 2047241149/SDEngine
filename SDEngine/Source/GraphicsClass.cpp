#include"GraphicsClass.h"

GraphicsClass::GraphicsClass(int ScreenWidth, int ScreenHeight, HWND hwnd, HINSTANCE hinstance)
{
	Initialize(ScreenWidth, ScreenHeight, hwnd, hinstance);
}

GraphicsClass::~GraphicsClass()
{
	CloseDebugConsole();
}

GraphicsClass::GraphicsClass(const GraphicsClass&other)
{

}

bool GraphicsClass::Initialize(int ScreenWidth, int ScreenHeight, HWND hwnd,HINSTANCE hinstance)
{

	mhwnd = hwnd;

	m_nScreenWidth = ScreenWidth;
	m_nScreenHeight = ScreenHeight;

	//创建D3DClass类并且初始化,D3DClass是第一个被创建并且初始化
	GDirectxCore->Init(ScreenWidth, ScreenHeight, VSYNC_ENABLE, hwnd, FULL_SCREEN, SCREEN_NEAR, SCREEN_FAR);

	//Console的Debug窗口
	InitDebugConsole();

	//创建相机
	GCamera->SetProjParams(XM_PI / 3.0f, (float)ScreenWidth / (float)ScreenHeight, SCREEN_NEAR, SCREEN_FAR);
	GCamera->SetUIOrthoParams((float)ScreenWidth, (float)ScreenHeight);

	//给游戏添加灯光 
	shared_ptr<DirectionLight> m_spDirLight = shared_ptr<DirectionLight>(new DirectionLight());
	m_spDirLight->SetLightPostion(XMFLOAT3(12.0f, 12.0f, 12.0f));
	m_spDirLight->SetLookAtPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_spDirLight->SetAmbientLight(XMFLOAT3(0.01f, 0.01f, 0.01f));
	m_spDirLight->SetLightColor(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	for (int nNum = -3; nNum < 3; ++nNum)
	{
		shared_ptr<PointLight> m_spPointLight = shared_ptr<PointLight>(new PointLight());
		m_spPointLight->SetLightColor(XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f));
		m_spPointLight->SetRadius(20.0f);
		m_spPointLight->SetLightAttenuation(XMFLOAT3(1.0f, 1.5f, 2.0f));
		m_spPointLight->SetLightPostion(XMFLOAT3(20.0f * nNum, 2.0f, 0.0f));
		GLightManager->Add(m_spPointLight);
	}

	GLightManager->Add(m_spDirLight);
	

	//创建mesh
	//(1)
	shared_ptr<Mesh> pHeadMesh = shared_ptr<Mesh>(new Mesh("Resource\\FBXModel\\head\\head.FBX"));
	pHeadMesh->m_eMaterialType = MaterialType::DIFFUSE;


	shared_ptr<Mesh> pOpacitySphereMesh = shared_ptr<Mesh>(new Mesh("Resource\\FBXModel\\sphere\\sphere.FBX"));
	pOpacitySphereMesh->m_eMaterialType = MaterialType::PURE_COLOR;
	pOpacitySphereMesh->pureColor = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

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
	mHeadObject->m_pTransform->localPosition = XMFLOAT3(0.0f, 10.0f, 0.0f);
	mHeadObject->m_pTransform->localRotation = XMFLOAT3(0.0f, 90.0f, 0.0f);
	mHeadObject->m_pTransform->localScale = XMFLOAT3(5.0f, 5.0f, 5.0f);

	mOpacitySphereObject = shared_ptr<GameObject>(new GameObject());
	mOpacitySphereObject->SetMesh(pOpacitySphereMesh);
	mOpacitySphereObject->m_pTransform->localScale = XMFLOAT3(1.0f, 1.0f, 1.0f);

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

	//创建输入类
	mInputClass = shared_ptr<Input>(new Input(hinstance, hwnd, ScreenWidth, ScreenHeight));

	mSrcRT = shared_ptr<RenderTexture>(
		new RenderTexture(ScreenWidth, ScreenHeight));

	mSSRRT = shared_ptr<RenderTexture>(
		new RenderTexture(ScreenWidth, ScreenHeight));

	mLightBuffer = shared_ptr<RenderTexture>(new RenderTexture(ScreenWidth, ScreenHeight));

	mDownSampleLightBuffer = shared_ptr<RenderTexture>(new RenderTexture(ScreenWidth / LIGHT_MAP_DOWN_SMAPLE, ScreenHeight/ LIGHT_MAP_DOWN_SMAPLE));

	mGeometryBuffer = shared_ptr<GeometryBuffer>(new 
		GeometryBuffer(ScreenWidth,ScreenHeight,SCREEN_FAR,SCREEN_NEAR));

	mQuad = shared_ptr<Quad>(new Quad());

	mDebugWindow = shared_ptr<DebugWindow>(new DebugWindow(ScreenWidth,ScreenHeight,120,120));

	mBackDepthBufferRT = shared_ptr<DepthBufferRT>(new DepthBufferRT(ScreenWidth, ScreenHeight));

	mSSRBuffer = shared_ptr<SSRGBuffer>(new 
		SSRGBuffer(ScreenWidth, ScreenHeight, SCREEN_FAR, SCREEN_NEAR));

	
	return true;
}


bool GraphicsClass::Frame()
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
		if (rotateY <= 90.0f&&rotateY >= -90.0f)
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

	GCamera->UpdateViewMatrix();
	//如果按下ESC，则破坏窗口
	if (mInputClass->IsEscapePressed())
	{
		DestroyWindow(mhwnd);
	}

	GCamera->UpdateViewMatrix();

	return true;
}


void GraphicsClass::Render()
{
	//**************************************************************************
	//绘制整个场景
	//*************************************************************************

	GDirectxCore->BeginScene(0.3f, 0.0f, 1.0f, 1.0f);

	//绘制不透明物体
	RenderOpacity();

	//获取整个场景的BackDepthBuffer
	#if defined(SSR)
	RenderSceneBackDepthBuffer();
	#endif
	

	//绘制透明物体(普通的透明物体，SSR)
	//RenderTransparency();

	/*#if defined(POST_EFFECT)
		RenderPostEffectPass();
	#endif // POST_EFFECT*/

	#if defined(DEBUG_GBUFFER)
		RenderDebugWindow();
	#endif


	//**************************************************************************
	//结束绘制
	//*************************************************************************
	GDirectxCore->EndScene();
}

void GraphicsClass::RenderGeometryPass()
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

void GraphicsClass::RenderLightingPass()
{
	RenderDirLightPass();
	RenderPointLightPass();

	GDirectxCore->SetBackBufferRender();
	GDirectxCore->SetViewPort();
	GDirectxCore->TurnOffZBuffer();
	GShaderManager->SetGraphcisBlitShader(mSrcRT->GetSRV());
	mQuad->Render();

	GDirectxCore->TurnOnZBuffer();
}



void GraphicsClass::RenderPostEffectPass()
{

}


void GraphicsClass::RenderDebugWindow()
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

	#if defined(SSR)

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

void GraphicsClass::RenderSSRPass()
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

void GraphicsClass::RenderOpacity()
{
	RenderGeometryPass();

	RenderLightingPass();
}

//绘制透明物体分为绘制透明
void GraphicsClass::RenderTransparency()
{
	RenderGeneralTransparency();

	#if defined(SSR)
	RenderSSR();
	#endif
}

void GraphicsClass::RenderGeneralTransparency()
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


void GraphicsClass::RenderSceneBackDepthBuffer()
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

void GraphicsClass::RenderSSRBufferPass()
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

void  GraphicsClass::RenderSSR()
{
	RenderSSRBufferPass();
	RenderSSRPass();
}


void GraphicsClass::InitDebugConsole()
{
	AllocConsole();
	FILE* pf;
	freopen_s(&pf, "CONOUT$", "w", stdout);   // 重定向输出
}


void GraphicsClass::CloseDebugConsole()
{
	FreeConsole();
}


void GraphicsClass::RenderPointLightPass()
{

	ID3D11RenderTargetView* backRTV[1] = { nullptr };
	ID3D11DepthStencilView* opacityDSV = mGeometryBuffer->GetDSV();
	ID3D11RenderTargetView* pSceneRTV = mSrcRT->GetRenderTargetView();
	ID3D11RenderTargetView* pLightRTV = mLightBuffer->GetRenderTargetView();
	mLightBuffer->ClearRenderTarget(0.0f, 0.0f, 0.0f, 1.0f);
	
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


	//对LightBuffer降采样
	GDirectxCore->TurnOffZBuffer();
	mDownSampleLightBuffer->SetRenderTarget();
	GShaderManager->SetGraphcisBlitShader(mLightBuffer->GetSRV());
	mQuad->Render();
	GDirectxCore->RecoverDefaultDSS();

	GDirectxCore->TurnOffZBuffer();
	GDirectxCore->SetViewPort();
	g_pDeviceContext->OMSetRenderTargets(1, &pSceneRTV, opacityDSV);
	ID3D11ShaderResourceView* pShaderViewArray[2];
	GDirectxCore->TurnOnLightBlend();
	pShaderViewArray[0] = mGeometryBuffer->GetGBufferSRV(GBufferType::Diffuse);
	pShaderViewArray[1] = mDownSampleLightBuffer->GetSRV();
	GShaderManager->SetDefferedFinalShader(pShaderViewArray);
	mQuad->Render();
	GDirectxCore->TurnOffAlphaBlend();
	GDirectxCore->RecoverDefaultDSS();
	
}


void GraphicsClass::RenderDirLightPass()
{

	mSrcRT->SetRenderTarget();
	ID3D11ShaderResourceView* shaderResourceView[4];
	shaderResourceView[0] = mGeometryBuffer->GetGBufferSRV(GBufferType::Diffuse);
	shaderResourceView[1] = mGeometryBuffer->GetGBufferSRV(GBufferType::Pos);
	shaderResourceView[2] = mGeometryBuffer->GetGBufferSRV(GBufferType::Normal);
	shaderResourceView[3] = mGeometryBuffer->GetGBufferSRV(GBufferType::Specular);

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