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
	//创建D3DClass类并且初始化,D3DClass应该是第一个被创建并且初始化
	D3DClass::GetInstance()->Initialize(ScreenWidth, ScreenHeight, VSYNC_ENABLE, hwnd, FULL_SCREEN, SCREEN_NEAR, SCREEN_FAR);

	ID3D11Device* d3dDevice = D3DClass::GetInstance()->GetDevice();

	InitDebugConsole();

	//创建相机
	Camera* mainCamera = Camera::GetInstance();
	mainCamera->SetProjParams(XM_PI / 3.0f, (float)ScreenWidth / (float)ScreenHeight, SCREEN_NEAR, SCREEN_FAR);
	mainCamera->SetUIOrthoParams((float)ScreenWidth, (float)ScreenHeight);

	//创建Shader管理器
	ShaderManager* mShaderManager = ShaderManager::GetInstance();

	//创建directionLight(游戏中仅仅存在一个方向光，用来模拟太阳光)
	Light* dirLight = Light::GetInstnce();
	XMVECTOR lightPos = XMVectorSet(12.0f, 12.0f, 12.0f, 0.0f);
	XMVECTOR lookAtPos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	dirLight->SetViewParams(lightPos, lookAtPos);
	dirLight->SetLightColor(XMVectorSet(1.0, 1.0f, 1.0f, 1.0f));
	dirLight->SetAmbientLight(XMVectorSet(0.05f, 0.05f, 0.05f, 1.0f));

	//创建ModelClasss
	mHeadObject = shared_ptr<GameObject>(new GameObject("FBXModel\\head\\head.FBX"));

	mSphereObject = shared_ptr<GameObject>(new GameObject("FBXModel\\sphere\\sphere.FBX"));

	mSponzaBottom = shared_ptr<GameObject>(new GameObject("FBXModel\\sponza\\sponza_bottom.FBX"));

	mSponzaNoBottom = shared_ptr<GameObject>(new GameObject("FBXModel\\sponza\\sponza_no_bottom.FBX"));

	//创建输入类
	mInputClass = shared_ptr<Input>(new Input(hinstance, hwnd, ScreenWidth, ScreenHeight));


	mSrcRT = shared_ptr<RenderTexture>(
		new RenderTexture(ScreenWidth, ScreenHeight));

	mSSRRT = shared_ptr<RenderTexture>(
		new RenderTexture(ScreenWidth, ScreenHeight));

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
	Camera* mainCamera = Camera::GetInstance();
	FPS* fpsPtr = FPS::GetInstance();
	fpsPtr->Frame();

	int fps = fpsPtr->GetFPS();

	deltaTime = fpsPtr->GetDeltaTime();
	float currentTime = fpsPtr->GetTime();
	mInputClass->GetMousePositionOffset(mouseXOffset, mouseYOffset);


	//鼠标右键处于按下的状态才能进行（左右移动）（前后移动）（旋转的操作）
	if (mInputClass->IsMouseRightButtuonPressed()&& fps >=5&& fps <=1000000)
	{
		//"W","S"键操作
		if (mInputClass->IsWPressed())
		{
			mainCamera->Walk(deltaTime*CAMERA_SPEED);
		}
		else if (mInputClass->IsSPressed())
		{
			mainCamera->Walk(-deltaTime*CAMERA_SPEED);
		}

		//"A","D"键操作
		if (mInputClass->IsAPressed())
		{
			mainCamera->Strafe(-deltaTime*CAMERA_SPEED);
		}
		else if (mInputClass->IsDPressed())
		{
			mainCamera->Strafe(deltaTime*CAMERA_SPEED);
		}

		//"Q","E"键操作
		if (mInputClass->IsQPressed())
		{
			mainCamera->UpDown(-deltaTime*CAMERA_SPEED);
		}
		else if (mInputClass->IsEPressed())
		{
			mainCamera->UpDown(deltaTime*CAMERA_SPEED);
		}


		//进行视角上下的旋转(跟刚开始的旋转角度在正负90度之间)
		if (rotateY <= 90.0f&&rotateY >= -90.0f)
		{
			rotateY += (float)mouseYOffset*deltaTime;
			mainCamera->Pitch((float)mouseYOffset*deltaTime*2.0f);
		}
	
		//进行视角左右的旋转
		mainCamera->RotateY((float)mouseXOffset*deltaTime*2.0f);
	}


	//1与2切换渲染模式
	if (mInputClass->IsKeyDown(DIK_1))
	{
		D3DClass::GetInstance()->TurnOnSolidRender();
	}
	if (mInputClass->IsKeyDown(DIK_2))
	{
		D3DClass::GetInstance()->TurnOnWireFrameRender();
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

	mainCamera->UpdateViewMatrix();
	//如果按下ESC，则破坏窗口
	if (mInputClass->IsEscapePressed())
	{
		DestroyWindow(mhwnd);
	}

	mainCamera->UpdateViewMatrix();

	return true;
}


void GraphicsClass::Render()
{
	//**************************************************************************
	//绘制整个场景
	//*************************************************************************

	auto d3dPtr = D3DClass::GetInstance();

	d3dPtr->BeginScene(0.3f, 0.0f, 1.0f, 1.0f);

	//绘制不透明物体
	RenderOpacity();

	//获取整个场景的BackDepthBuffer
	//RenderSceneBackDepthBuffer();

	//绘制透明物体(普通的透明物体，SSR)
	RenderTransparency();

	#if defined(POST_EFFECT)
		RenderPostEffectPass();
	#endif // POST_EFFECT

	#if defined(DEBUG_GBUFFER)
		RenderDebugWindow();
	#endif


	//**************************************************************************
	//结束绘制
	//*************************************************************************
	d3dPtr->EndScene();
}



void GraphicsClass::RenderFBXMesh()
{

	//渲染头
	mHeadObject->mTransform->localScale = XMFLOAT3(5.0f, 5.0f, 5.0f);
	mHeadObject->mTransform->localPosition = XMFLOAT3(0.0f, 10.0f, 0.0f);
	mHeadObject->mTransform->localRotation = XMFLOAT3(0.0f, 90.0f, 0.0f);
	mHeadObject->Render(materialType);

	//渲染SponzaBottom(用于SSR反射)
	D3DClass::GetInstance()->TurnOnMaskReflectDSS();
	mSponzaBottom->mTransform->localPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mSponzaBottom->Render(MaterialType::DIFFUSE);
	D3DClass::GetInstance()->TurnOnZBuffer();//恢复默认的DSS

	//渲染SponzaNoBottm
	mSponzaNoBottom->mTransform->localPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mSponzaNoBottom->Render(MaterialType::DIFFUSE);

	//球渲染
	mSphereObject->mTransform->localScale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	//平行光源球
	 XMStoreFloat3(&mSphereObject->mTransform->localPosition,
		 -Light::GetInstnce()->GetLightDirection());
	mSphereObject->Render(MaterialType::PURE_COLOR, XMVectorSet(1.0f, 1.0f, 0.0f, 1.0f));
	
	//原点球
	mSphereObject->mTransform->localPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mSphereObject->Render(MaterialType::PURE_COLOR,XMVectorSet(1.0f,1.0f,0.0f,1.0f));

}

void GraphicsClass::RenderGeometryPass()
{
	mGeometryBuffer->SetRenderTarget(XMFLOAT3(0.0f, 0.0f, 0.5f));

	RenderFBXMesh();
}

void GraphicsClass::RenderLightingPass()
{
	mSrcRT->SetRenderTarget();
	D3DClass::GetInstance()->TurnOffZBuffer();
	ID3D11ShaderResourceView* shaderResourceView[4];
	shaderResourceView[0] = mGeometryBuffer->GetGBufferSRV(GBufferType::Diffuse);
	shaderResourceView[1] = mGeometryBuffer->GetGBufferSRV(GBufferType::Pos);
	shaderResourceView[2] = mGeometryBuffer->GetGBufferSRV(GBufferType::Normal);
	shaderResourceView[3] = mGeometryBuffer->GetGBufferSRV(GBufferType::Specular);
	ShaderManager::GetInstance()->SetDefferLighingShader(shaderResourceView);
	mQuad->Render();

	D3DClass::GetInstance()->SetBackBufferRender();
	D3DClass::GetInstance()->SetViewPort();
	ShaderManager::GetInstance()->SetGraphcisBlitShader(mSrcRT->GetSRV());
	mQuad->Render();

	D3DClass::GetInstance()->TurnOnZBuffer();
}



void GraphicsClass::RenderPostEffectPass()
{

}


void GraphicsClass::RenderDebugWindow()
{
	D3DClass::GetInstance()->SetBackBufferRender();
	D3DClass::GetInstance()->SetViewPort();
	D3DClass::GetInstance()->TurnOffZBuffer();

	//diffuse
	ShaderManager::GetInstance()->SetUIShader
	(mGeometryBuffer->GetGBufferSRV(GBufferType::Diffuse));
	mDebugWindow->Render(10, 600);

	//pos
	ShaderManager::GetInstance()->SetUIShader
	(mGeometryBuffer->GetGBufferSRV(GBufferType::Pos));
	mDebugWindow->Render(130, 600);

	//normal
	ShaderManager::GetInstance()->SetUIShader
	(mGeometryBuffer->GetGBufferSRV(GBufferType::Normal));
	mDebugWindow->Render(250, 600);

	//specular
	ShaderManager::GetInstance()->SetUIShader
	(mGeometryBuffer->GetGBufferSRV(GBufferType::Specular));
	mDebugWindow->Render(370, 600);

	ShaderManager::GetInstance()->SetDepthShader
	(mGeometryBuffer->GetGBufferSRV(GBufferType::Depth));
	mDebugWindow->Render(490, 600);


	ShaderManager::GetInstance()->SetDepthShader
	(mBackDepthBufferRT->GetShaderResourceView());
	mDebugWindow->Render(610, 600);

	ShaderManager::GetInstance()->SetUIShader
	(mSSRBuffer->GetGBufferSRV(SSRBufferType::VIEW_NORMAL));
	mDebugWindow->Render(10, 480);

	ShaderManager::GetInstance()->SetUIShader
	(mSSRBuffer->GetGBufferSRV(SSRBufferType::VIEW_POS));
	mDebugWindow->Render(10, 360);

	D3DClass::GetInstance()->TurnOnZBuffer();
}

void GraphicsClass::RenderSSRPass()
{
	//mSSRRT->SetRenderTarget();
	ID3D11DeviceContext* d3dDeviceContext = D3DClass::GetInstance()->GetDeviceContext();
	ID3D11RenderTargetView* backRTV = D3DClass::GetInstance()->GetRTV();
	//靠模板緩存值來判斷
	d3dDeviceContext->OMSetRenderTargets(1, &backRTV, nullptr);
	D3DClass::GetInstance()->SetViewPort();
	D3DClass::GetInstance()->TurnOnAlphaBlend();
	D3DClass::GetInstance()->TurnOffZBuffer();
	XMMATRIX worldMatrix = mSponzaBottom->GetWorldMatrix();
	XMMATRIX projMatrix = Camera::GetInstance()->GetProjectionMatrix();
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
	ShaderManager::GetInstance()->SetSSRShader(worldMatrix,arraySRV,perspectiveValues);
	mQuad->Render();
	D3DClass::GetInstance()->TurnOnZBuffer();
	D3DClass::GetInstance()->TurnOffAlphaBlend();
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
	ID3D11DeviceContext* d3dDeviceContext = D3DClass::GetInstance()->GetDeviceContext();
	ID3D11RenderTargetView* backRTV = D3DClass::GetInstance()->GetRTV();
	ID3D11DepthStencilView* opacityDSV = mGeometryBuffer->GetDSV();
	d3dDeviceContext->OMSetRenderTargets(1, &backRTV, opacityDSV);
	D3DClass::GetInstance()->SetViewPort();
	D3DClass::GetInstance()->TurnOnDisbleZWriteDSS();
	D3DClass::GetInstance()->TurnOnAlphaBlend();

	mSphereObject->mTransform->localPosition = XMFLOAT3(3.0, 9.0, 0.0);
	mSphereObject->mTransform->localScale = XMFLOAT3(3.0, 3.0, 3.0);
	XMMATRIX worldMatrix = mSphereObject->GetWorldMatrix();
	ShaderManager::GetInstance()->SetForwardPureColorShader(worldMatrix, XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f));
	mSphereObject->RenderMesh();
	D3DClass::GetInstance()->TurnOffAlphaBlend();
	D3DClass::GetInstance()->RecoverDefaultDSS();
}


void GraphicsClass::RenderSceneBackDepthBuffer()
{
	mBackDepthBufferRT->SetRenderTarget();
	D3DClass::GetInstance()->TurnOnCullFront();

	//渲染头
	mHeadObject->mTransform->localScale = XMFLOAT3(5.0f, 5.0f, 5.0f);
	mHeadObject->mTransform->localPosition = XMFLOAT3(0.0f, 10.0f, 0.0f);
	mHeadObject->mTransform->localRotation = XMFLOAT3(0.0f, 90.0f, 0.0f);
	mHeadObject->Render(MaterialType::DEPTH_BUFFER);

	//渲染SponzaBottom
	mSponzaBottom->mTransform->localPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mSponzaBottom->Render(MaterialType::DEPTH_BUFFER);

    //渲染SponzaNoBottm
	mSponzaNoBottom->mTransform->localPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mSponzaNoBottom->Render(MaterialType::DEPTH_BUFFER);

	//球渲染
	mSphereObject->mTransform->localScale = XMFLOAT3(1.0f, 1.0f, 1.0f);


	//平行光源球
	XMStoreFloat3(&mSphereObject->mTransform->localPosition,
		-Light::GetInstnce()->GetLightDirection());
	mSphereObject->Render(MaterialType::DEPTH_BUFFER);

	//原点球
	mSphereObject->mTransform->localPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mSphereObject->Render(MaterialType::DEPTH_BUFFER);


	//恢复默认的RS
	D3DClass::GetInstance()->TurnOnSolidRender();

}

void GraphicsClass::RenderSSRBufferPass()
{
	ID3D11DepthStencilView* backDSV = mGeometryBuffer->GetDSV();
	mSSRBuffer->SetRenderTarget(backDSV);
	D3DClass::GetInstance()->TurnOnEnableReflectDSS();
	ID3D11ShaderResourceView* arraySRV[2];
	arraySRV[0] = mGeometryBuffer->GetGBufferSRV(GBufferType::Pos);
	arraySRV[1] = mGeometryBuffer->GetGBufferSRV(GBufferType::Normal);
	ShaderManager::GetInstance()->SetSSRGBufferShader(arraySRV);
	mQuad->Render();
	D3DClass::GetInstance()->RecoverDefaultDSS();
	D3DClass::GetInstance()->SetBackBufferRender();

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
