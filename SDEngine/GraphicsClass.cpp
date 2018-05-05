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

	//����D3DClass�ಢ�ҳ�ʼ��,D3DClassӦ���ǵ�һ�����������ҳ�ʼ��
	D3DClass::GetInstance()->Initialize(ScreenWidth, ScreenHeight, VSYNC_ENABLE, hwnd, FULL_SCREEN, SCREEN_NEAR, SCREEN_FAR);

	ID3D11Device* d3dDevice = D3DClass::GetInstance()->GetDevice();


	//�������
	Camera* mainCamera = Camera::GetInstance();
	mainCamera->SetProjParams(XM_PI / 3.0f, (float)ScreenWidth / (float)ScreenHeight, 0.1f, 400.0f);
	mainCamera->SetUIOrthoParams(ScreenWidth, ScreenHeight);

	//����Shader������
	ShaderManager* mShaderManager = ShaderManager::GetInstance();

	//����directionLight(��Ϸ�н�������һ������⣬����ģ��̫����)
	Light* dirLight = Light::GetInstnce();
	XMVECTOR lightPos = XMVectorSet(6.0f, 6.0f, 6.0f, 0.0f);
	XMVECTOR lookAtPos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	dirLight->SetViewParams(lightPos, lookAtPos);
	dirLight->SetLightColor(XMVectorSet(1.0, 1.0f, 1.0f, 1.0f));
	dirLight->SetAmbientLight(XMVectorSet(0.15f, 0.15f, 0.15f, 1.0f));

	//����ModelClasss
	mHeadObject = shared_ptr<GameObject>(new GameObject("FBXModel\\head\\head.FBX"));

	mSphereObject = shared_ptr<GameObject>(new GameObject("FBXModel\\sphere\\sphere.FBX"));

	mSponzaBottom = shared_ptr<GameObject>(new GameObject("FBXModel\\sponza\\sponza_bottom.FBX"));

	mSponzaNoBottom = shared_ptr<GameObject>(new GameObject("FBXModel\\sponza\\sponza_no_bottom.FBX"));
	
	//����������
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

	mSSRRT = shared_ptr<ColorBufferRT>(
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

	//����InputClass�����֡����
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


	//����Ҽ����ڰ��µ�״̬���ܽ��У������ƶ�����ǰ���ƶ�������ת�Ĳ�����
	if (mInputClass->IsMouseRightButtuonPressed()&& fps >=5&& fps <=1000000)
	{
		//"W","S"������
		if (mInputClass->IsWPressed())
		{
			mainCamera->Walk(deltaTime*CAMERA_SPEED);
		}
		else if (mInputClass->IsSPressed())
		{
			mainCamera->Walk(-deltaTime*CAMERA_SPEED);
		}

		//"A","D"������
		if (mInputClass->IsAPressed())
		{
			mainCamera->Strafe(-deltaTime*CAMERA_SPEED);
		}
		else if (mInputClass->IsDPressed())
		{
			mainCamera->Strafe(deltaTime*CAMERA_SPEED);
		}

		//"Q","E"������
		if (mInputClass->IsQPressed())
		{
			mainCamera->UpDown(-deltaTime*CAMERA_SPEED);
		}
		else if (mInputClass->IsEPressed())
		{
			mainCamera->UpDown(deltaTime*CAMERA_SPEED);
		}


		//�����ӽ����µ���ת(���տ�ʼ����ת�Ƕ�������90��֮��)
		if (rotateY <= 90.0f&&rotateY >= -90.0f)
		{
			rotateY += (float)mouseYOffset*deltaTime;
			mainCamera->Pitch((float)mouseYOffset*deltaTime*2.0f);
		}
	
		//�����ӽ����ҵ���ת
		mainCamera->RotateY((float)mouseXOffset*deltaTime*2.0f);
	}


	//1��2�л���Ⱦģʽ
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
	//�������ESC�����ƻ�����
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
	//������������
	//*************************************************************************

	auto d3dPtr = D3DClass::GetInstance();

	d3dPtr->BeginScene(0.3f, 0.0f, 1.0f, 1.0f);

	//���Ʋ�͸������
	RenderOpacity();

	//����͸������(��ͨ��͸�����壬SSR)
	RenderTransparency();

	#if defined(POST_EFFECT)
		RenderPostEffectPass();
	#endif // POST_EFFECT

	RenderDebugWindow();

	//**************************************************************************
	//��������
	//*************************************************************************
	d3dPtr->EndScene();
}



void GraphicsClass::RenderFBXMesh()
{

	//��Ⱦͷ
	mHeadObject->mTransform->localScale = XMFLOAT3(5.0f, 5.0f, 5.0f);
	mHeadObject->mTransform->localPosition = XMFLOAT3(0.0f, 10.0f, 0.0f);
	mHeadObject->mTransform->localRotation = XMFLOAT3(0.0f, 90.0f, 0.0f);
	mHeadObject->Render(materialType);

	//��ȾSponzaBottom(����SSR����)
	D3DClass::GetInstance()->TurnOnMaskReflectDSS();
	mSponzaBottom->mTransform->localPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mSponzaBottom->Render(MaterialType::DIFFUSE);
	D3DClass::GetInstance()->TurnOnZBuffer();//�ָ�Ĭ�ϵ�DSS

	//��ȾSponzaNoBottm
	mSponzaNoBottom->mTransform->localPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mSponzaNoBottom->Render(MaterialType::DIFFUSE);

	//����Ⱦ
	mSphereObject->mTransform->localScale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	//ƽ�й�Դ��
	 XMStoreFloat3(&mSphereObject->mTransform->localPosition,
		 -Light::GetInstnce()->GetLightDirection());
	mSphereObject->Render(MaterialType::PURE_COLOR, XMVectorSet(1.0f, 1.0f, 0.0f, 1.0f));
	
	//ԭ����
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
	//1.������
	//2.����ģ��
	//3.������
	/********************/
	D3DClass::GetInstance()->TurnOffZBuffer();
	//��srcRT��ΪdownSampleRT
	mDownSampleRT->SetRenderTarget();
	ShaderManager::GetInstance()->SetGraphcisBlitShader(mSrcRT->GetShaderResourceView());
	mQuad->Render();

	//��downSampleRT��ΪmFirstBlurRT
	mFirstBlurRT->SetRenderTarget();
	ShaderManager::GetInstance()->SetBlurShader(mDownSampleRT->GetShaderResourceView());
	mQuad->Render();

	//��mFirstBlurRT��ΪmSecondBlurRT
	mSceondBlurRT->SetRenderTarget();
	ShaderManager::GetInstance()->SetBlurShader(mFirstBlurRT->GetShaderResourceView());
	mQuad->Render();

	//��blurSampleRT��ΪupSampleRT
	mUpSampleRT->SetRenderTarget();
	ShaderManager::GetInstance()->SetGraphcisBlitShader(mSceondBlurRT->GetShaderResourceView());
	mQuad->Render();

	//��UpSample��ΪBlurRT
	D3DClass::GetInstance()->SetBackBufferRender();
	D3DClass::GetInstance()->SetViewPort();
	ShaderManager::GetInstance()->SetDOFShader(mSrcRT->GetShaderResourceView(),
		mUpSampleRT->GetShaderResourceView()
	,mGeometryBuffer->GetGBufferSRV(GBufferType::Depth),35.0f,70.0f,SCREEN_FAR,SCREEN_NEAR);
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

	ShaderManager::GetInstance()->SetUIShader
	(mSSRRT->GetShaderResourceView());
	mDebugWindow->Render(490, 600);

	D3DClass::GetInstance()->TurnOnZBuffer();
}

void GraphicsClass::RenderSSRPass()
{
	//mSSRRT->SetRenderTarget();
	ID3D11DeviceContext* d3dDeviceContext = D3DClass::GetInstance()->GetDeviceContext();
	ID3D11RenderTargetView* backRTV = D3DClass::GetInstance()->GetRTV();
	ID3D11DepthStencilView* opacityDSV = mGeometryBuffer->GetDSV();
	d3dDeviceContext->OMSetRenderTargets(1, &backRTV, opacityDSV);
	D3DClass::GetInstance()->SetViewPort();
	D3DClass::GetInstance()->TurnOnAlphaBlend();
	D3DClass::GetInstance()->TurnOnEnableReflectDSS();
	
	XMMATRIX worldMatrix = mSponzaBottom->GetWorldMatrix();
	
	float viewAngleThresshold = 0.05f;
	float edgeDistThresshold = 0.45;
	float depthBias = 0.5f;
	float reflectScale = 1.0f;
	XMMATRIX projMatrix = Camera::GetInstance()->GetProjectionMatrix();
	XMFLOAT4X4 projFloat4X4;
	XMStoreFloat4x4(&projFloat4X4, projMatrix);
	XMFLOAT4 perspectiveValues;
	perspectiveValues.x = 1.0f / projFloat4X4.m[0][0];
	perspectiveValues.y = 1.0f / projFloat4X4.m[1][1];
	perspectiveValues.z = projFloat4X4.m[3][2];
	perspectiveValues.w = -projFloat4X4.m[2][2];

	ShaderManager::GetInstance()->SetSSRShader(worldMatrix,
		mSrcRT->GetShaderResourceView(), mGeometryBuffer->GetGBufferSRV(GBufferType::Depth),
		viewAngleThresshold,edgeDistThresshold,depthBias,reflectScale,perspectiveValues);

	mSponzaBottom->RenderMesh();

	D3DClass::GetInstance()->TurnOffAlphaBlend();
	D3DClass::GetInstance()->TurnOnZBuffer();
}

void GraphicsClass::RenderOpacity()
{
	RenderGeometryPass();

	RenderLightingPass();
}

//����͸�������Ϊ����͸��
void GraphicsClass::RenderTransparency()
{
	RenderGeneralTransparency();
	RenderSSRPass();
}

void GraphicsClass::RenderGeneralTransparency()
{
	ID3D11DeviceContext* d3dDeviceContext = D3DClass::GetInstance()->GetDeviceContext();
	ID3D11RenderTargetView* backRTV = D3DClass::GetInstance()->GetRTV();
	ID3D11DepthStencilView* opacityDSV = mGeometryBuffer->GetDSV();
	d3dDeviceContext->OMSetRenderTargets(1, &backRTV, opacityDSV);
	D3DClass::GetInstance()->SetViewPort();
	D3DClass::GetInstance()->TurnOnAlphaBlend();


	mSphereObject->mTransform->localPosition = XMFLOAT3(3.0, 9.0, 0.0);
	mSphereObject->mTransform->localScale = XMFLOAT3(3.0, 3.0, 3.0);
	XMMATRIX worldMatrix = mSphereObject->GetWorldMatrix();
	ShaderManager::GetInstance()->SetForwardPureColorShader(worldMatrix, XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f));
	mSphereObject->RenderMesh();
	D3DClass::GetInstance()->TurnOffAlphaBlend();

}