#include"GraphicsClass.h"

GraphicsClass::GraphicsClass(int ScreenWidth, int ScreenHeight, HWND hwnd, HINSTANCE hinstance)
{
	Initialize(ScreenWidth, ScreenHeight, hwnd, hinstance);
}

GraphicsClass::~GraphicsClass()
{

}

GraphicsClass::GraphicsClass(const GraphicsClass&other)
{

}

bool GraphicsClass::Initialize(int ScreenWidth, int ScreenHeight, HWND hwnd,HINSTANCE hinstance)
{

	mhwnd = hwnd;

	//创建D3DClass类并且初始化,D3DClass应该是第一个被创建并且初始化
	D3DClass::GetInstance()->Initialize(ScreenWidth, ScreenHeight, VSYNC_ENABLE, hwnd, FULL_SCREEN, SCREEN_NEAR, SCREEN_FAR);

	ID3D11Device* d3dDevice = D3DClass::GetInstance()->GetDevice();


	//创建相机
	Camera* mainCamera = Camera::GetInstance();
	mainCamera->SetProjParams(XM_PI / 3.0f, (float)ScreenWidth / (float)ScreenHeight, 0.1f, 400.0f);
	mainCamera->SetUIOrthoParams(ScreenWidth, ScreenHeight);
	//mainCamera->SetPosition(0.0f, 0.0f, 0.0f);

	//创建Shader管理器
	ShaderManager* mShaderManager = ShaderManager::GetInstance();

	//创建directionLight(游戏中仅仅存在一个方向光，用来模拟太阳光)
	Light* dirLight = Light::GetInstnce();
	XMVECTOR lightPos = XMVectorSet(6.0f, 6.0f, 6.0f, 0.0f);
	XMVECTOR lookAtPos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	dirLight->SetViewParams(lightPos, lookAtPos);
	dirLight->SetLightColor(XMVectorSet(1.0, 1.0f, 1.0f, 1.0f));
	dirLight->SetAmbientLight(XMVectorSet(0.15f, 0.15f, 0.15f, 1.0f));

	//创建ModelClasss
	mHeadObject = shared_ptr<GameObject>(new GameObject("FBXModel\\head\\head.FBX"));

	mSphereObject = shared_ptr<GameObject>(new GameObject("FBXModel\\sphere\\sphere.FBX"));

	mSponzaObject = shared_ptr<GameObject>(new GameObject("FBXModel\\sponza\\sponza.FBX"));

	//创建输入类
	mInputClass = shared_ptr<Input>(new Input(hinstance, hwnd, ScreenWidth, ScreenHeight));

	mDownSampleRT = shared_ptr<ColorBufferRT>(
		new ColorBufferRT(512, 512, SCREEN_FAR, SCREEN_NEAR));

	mFirstBlurRT = shared_ptr<ColorBufferRT>(
		new ColorBufferRT(512, 512, SCREEN_FAR, SCREEN_NEAR));

	mSceondBlurRT = shared_ptr<ColorBufferRT>(
		new ColorBufferRT(512, 512, SCREEN_FAR, SCREEN_NEAR));

	mUpSampleRT = shared_ptr<ColorBufferRT>(
		new ColorBufferRT(1024, 1024, SCREEN_FAR, SCREEN_NEAR));

	mSrcRT = shared_ptr<ColorBufferRT>(
		new ColorBufferRT(ScreenWidth, ScreenHeight, SCREEN_FAR, SCREEN_NEAR));

	mGeometryBuffer = shared_ptr<GeometryBuffer>(new 
		GeometryBuffer(ScreenWidth,ScreenHeight,SCREEN_FAR,SCREEN_NEAR));

	mQuad = shared_ptr<Quad>(new Quad());


	mDebugWindow = shared_ptr<DebugWindow>(new DebugWindow(ScreenWidth,ScreenHeight,120,120));

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

	RenderGeometryPass();

	RenderLightingPass();

	#if defined(POST_EFFECT)
		RenderPostEffectPass();
	#endif // POST_EFFECT



	RenderDebugWindow();

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

	//渲染sponza
	mSponzaObject->mTransform->localPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mSponzaObject->Render(MaterialType::DIFFUSE);

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
	ShaderManager::GetInstance()->SetGraphcisBlitShader(mSrcRT->GetShaderResourceView());
	mQuad->Render();

	D3DClass::GetInstance()->TurnOnZBuffer();
}



void GraphicsClass::RenderPostEffectPass()
{
	/*********************/
	//1.降采样
	//2.两次模糊
	//3.升采样
	/********************/
	D3DClass::GetInstance()->TurnOffZBuffer();
	//将srcRT变为downSampleRT
	mDownSampleRT->SetRenderTarget();
	ShaderManager::GetInstance()->SetGraphcisBlitShader(mSrcRT->GetShaderResourceView());
	mQuad->Render();

	//将downSampleRT变为mFirstBlurRT
	mFirstBlurRT->SetRenderTarget();
	ShaderManager::GetInstance()->SetBlurShader(mDownSampleRT->GetShaderResourceView());
	mQuad->Render();

	//将mFirstBlurRT变为mSecondBlurRT
	mSceondBlurRT->SetRenderTarget();
	ShaderManager::GetInstance()->SetBlurShader(mFirstBlurRT->GetShaderResourceView());
	mQuad->Render();

	//将blurSampleRT变为upSampleRT
	mUpSampleRT->SetRenderTarget();
	ShaderManager::GetInstance()->SetGraphcisBlitShader(mSceondBlurRT->GetShaderResourceView());
	mQuad->Render();

	//将UpSample作为BlurRT
	D3DClass::GetInstance()->SetBackBufferRender();
	D3DClass::GetInstance()->SetViewPort();
	ShaderManager::GetInstance()->SetDOFShader(mSrcRT->GetShaderResourceView(),
		mUpSampleRT->GetShaderResourceView()
	,mGeometryBuffer->GetGBufferSRV(GBufferType::Depth),15.0f,50.0f,SCREEN_FAR,SCREEN_NEAR);
	mQuad->Render();

	D3DClass::GetInstance()->TurnOnZBuffer();
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

	D3DClass::GetInstance()->TurnOnZBuffer();
}