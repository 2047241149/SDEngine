#include "DirectxCore.h"

DirectxCore::DirectxCore():
	md3dDevice(nullptr),
	md3dImmediateContext(nullptr),
	md3dSwapChain(nullptr),
	md3dRenderTargetView(nullptr),
	md3dDepthStencilView(nullptr),
	md3dDepthStencilBuffer(nullptr),
	md3dDepthStencilState(nullptr),
	md3dDisableDepthStencilState(nullptr),
	md3dRasterizerState(nullptr),
	md3dEnableBlendState(nullptr),
	md3dDisableBlendState(nullptr),
	md3dWireFrameRS(nullptr),
	m_pLightBlendState(nullptr),
	renderSkyBoxDSS(nullptr)
{
	
}

DirectxCore::~DirectxCore()
{
	ShutDown();
}


DirectxCore::DirectxCore(const DirectxCore& other)
{
	ShutDown();
}

shared_ptr<DirectxCore> DirectxCore::Get()
{
	if (nullptr == m_pDirectxCore)
	{
		m_pDirectxCore = shared_ptr<DirectxCore>(new DirectxCore());
	}
	return m_pDirectxCore;
}


bool DirectxCore::Init(int ScreenWidth, int ScreenHeight, bool vsync, HWND hwnd, bool fullscreen, float ScreenDepth, float ScreenNear)
{
	
	//--------------------------------------------------------------
	//获取显示模式信息和显卡信息
	//---------------------------------------------------------------

	IDXGIAdapter* adpter;//适配器
	IDXGIFactory* factory;
	unsigned int numModes, numerator, denominator, stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;

	//存储vsyn设定
	mVsyncEnable = vsync;

	//创建一个Directx图形接口factory
	HR(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory));

	//使用factory来为显卡创建一个adapter
	HR(factory->EnumAdapters(0, &adpter));

	//列举主要的适配器输出
	HR(adpter->EnumOutputs(0, &mAdapterOutput));

	//获取适应适配器DXGI_FORMAT_R8G8B8A8_UNORM显示格式的模式数目
	HR(mAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL));

	//创建一个显示模式列表存放可能的显示模式(显卡,监视器)
	displayModeList= new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
		return false;

	//填充显示模式列表结构体
	HR(mAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList));

	//浏览所有的显示模式,找到适合屏幕宽度和高度的
	//当一个模式匹配,存储监视器刷新速度的分子分母??
	for (unsigned int i = 0; i<numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)ScreenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)ScreenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	//获取适配器(显卡)形容
	HR(adpter->GetDesc(&adapterDesc));

	//获取显卡内存量
	mVideoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	//将显卡名字转存在字符数组
	error = wcstombs_s(&stringLength, mVideoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	//释放显示模式列表
	delete[] displayModeList;
	displayModeList = NULL;
	ReleaseCOM(adpter);
	ReleaseCOM(factory);


	//-----------------------------------------------------
	//填充交换链形容结构体
	//-----------------------------------------------------
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));

	sd.BufferDesc.Width = ScreenWidth;
	sd.BufferDesc.Height = ScreenHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	if (mVsyncEnable) //限不限帧
	{
		sd.BufferDesc.RefreshRate.Numerator =numerator;
		sd.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		sd.BufferDesc.RefreshRate.Numerator = 0;
		sd.BufferDesc.RefreshRate.Denominator = 1;
	}
    
	//关闭多重采样
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	
	//是否进行全屏
	if (fullscreen)
	{
		sd.Windowed = false;  
	}
	else
	{
		sd.Windowed = true;
	}
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;  //背后缓存数量
	sd.OutputWindow = hwnd; //交换链所属的窗口
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	#if defined(_DEBUG)
	// If the project is in a debug build, enable the debug layer.
		sd.Flags = D3D11_CREATE_DEVICE_DEBUG;
	#endif
	
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//---------------------------------------------------------------
	//创建交换链和D3D设备和D3D设备上下文
	//---------------------------------------------------------------
	D3D_FEATURE_LEVEL featureLevel;
	featureLevel = D3D_FEATURE_LEVEL_11_1;
	HR(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &sd, &md3dSwapChain, &md3dDevice, NULL, &md3dImmediateContext));

	#if defined(DEBUG) | defined(_DEBUG)
		ID3D11Debug *pD3DDebug = NULL;
		if (SUCCEEDED(md3dDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<VOID**>(&pD3DDebug))))
		{
			ID3D11InfoQueue *d3dInfoQueue = nullptr;
			pD3DDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

			if (SUCCEEDED(pD3DDebug->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&d3dInfoQueue)))
			{
				d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
				d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
				d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_INFO, true);
				d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_MESSAGE, true);
				d3dInfoQueue->SetBreakOnCategory(D3D11_MESSAGE_CATEGORY_MISCELLANEOUS, true);
				d3dInfoQueue->SetBreakOnCategory(D3D11_MESSAGE_CATEGORY_INITIALIZATION, true);
				d3dInfoQueue->SetBreakOnCategory(D3D11_MESSAGE_CATEGORY_CLEANUP, true);
				d3dInfoQueue->SetBreakOnCategory(D3D11_MESSAGE_CATEGORY_COMPILATION, true);
				d3dInfoQueue->SetBreakOnCategory(D3D11_MESSAGE_CATEGORY_STATE_CREATION, true);
				d3dInfoQueue->SetBreakOnCategory(D3D11_MESSAGE_CATEGORY_STATE_SETTING, true);
				d3dInfoQueue->SetBreakOnCategory(D3D11_MESSAGE_CATEGORY_RESOURCE_MANIPULATION, true);
			}
		}
	#endif

	//--------------------------------------------------------------
	//第四,创建背后缓存视图
	//--------------------------------------------------------------
	ID3D11Texture2D*backBuffer;
	md3dSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	md3dDevice->CreateRenderTargetView(backBuffer, 0, &md3dRenderTargetView);
	ReleaseCOM(backBuffer);

	//--------------------------------------------------------------
	//填充2DTexture深度缓存(模板缓存)形容结构体，创建深度缓存(模板缓存)
	//--------------------------------------------------------------
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.Width = ScreenWidth;
	depthStencilDesc.Height = ScreenHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	HR(md3dDevice->CreateTexture2D(&depthStencilDesc,//要创建的纹理的形容
		0,
		&md3dDepthStencilBuffer)); //指向深度缓存的指针


	//-------------------------------------------------------------
	//创建并设定深度缓存(模板缓存)状态，指示如何使用Depth和stencil(Test)
	//-------------------------------------------------------------
	D3D11_DEPTH_STENCIL_DESC DSDESC;
	ZeroMemory(&DSDESC, sizeof(DSDESC));
	DSDESC.DepthEnable = true;
	DSDESC.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DSDESC.DepthFunc = D3D11_COMPARISON_LESS;
	DSDESC.StencilEnable = true;
	DSDESC.StencilReadMask = 0xff;
	DSDESC.StencilWriteMask = 0xff;
	//前面设定
	DSDESC.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DSDESC.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	DSDESC.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	DSDESC.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	//背面设定,在光栅化状态剔除背面时这个设定没用,但是依然要设定,不然无法创建深度(模板)状态
	DSDESC.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DSDESC.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	DSDESC.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DSDESC.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	HR(md3dDevice->CreateDepthStencilState(&DSDESC, &md3dDepthStencilState));
	md3dImmediateContext->OMSetDepthStencilState(md3dDepthStencilState, 0);

	//--------------------------------------------------------------
	//创建深度缓存(模板缓存)视图
	//--------------------------------------------------------------
	HR(md3dDevice->CreateDepthStencilView(
		md3dDepthStencilBuffer, //我们基于这个深度缓存/漏字板缓存创建一个视图
		0,
		&md3dDepthStencilView));//指向深度缓存/漏字板视图的指针


	//-------------------------------------------------------------
	//把那些视图绑定到输出合并阶段
	//-------------------------------------------------------------
	md3dImmediateContext->OMSetRenderTargets(1, &md3dRenderTargetView, md3dDepthStencilView);


	//正常的光栅化状态
	HR(DirectXFrame::CreateRasterizerState(md3dDevice, &md3dRasterizerState));

	//剔除前面
	HR(DirectXFrame::CreateRasterizerState(md3dDevice, &md3dCullFrontRS, D3D11_CULL_FRONT));

	//线框绘制
	HR(DirectXFrame::CreateRasterizerState(md3dDevice, &md3dWireFrameRS, D3D11_CULL_BACK, D3D11_FILL_WIREFRAME));

	//不进行剔除
	HR(DirectXFrame::CreateRasterizerState(md3dDevice, &m_pTurnOffCullBackRS, D3D11_CULL_NONE));

	//-------------------------------------------------------------
	//创建并设定视口
	//-------------------------------------------------------------
	mViewport.Width = static_cast<float>(ScreenWidth);
	mViewport.Height = static_cast<float>(ScreenHeight);
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;
	mViewport.TopLeftX = 0.0f;
	mViewport.TopLeftY = 0.0f;
	md3dImmediateContext->RSSetViewports(1,&mViewport);


	//第十二,输出一个Text文件保存显卡的信息 
	char CardInfo[128];
	int memory;
	GetVideoCardInfo(CardInfo, memory);
	ofstream os("I:/1.txt");
	os << "memory=" << memory << "  " << " CardInfo= " << CardInfo;


	//创建一个使ZBuffer无效的DepthStencilState状态
	D3D11_DEPTH_STENCIL_DESC DisableDepthDESC;
	ZeroMemory(&DisableDepthDESC, sizeof(DisableDepthDESC));
	DisableDepthDESC.DepthEnable = false;
	DisableDepthDESC.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DisableDepthDESC.DepthFunc = D3D11_COMPARISON_LESS;
	DisableDepthDESC.StencilEnable = true;
	DisableDepthDESC.StencilReadMask = 0xff;
	DisableDepthDESC.StencilWriteMask = 0xff;
	DisableDepthDESC.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DisableDepthDESC.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	DisableDepthDESC.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DisableDepthDESC.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	DisableDepthDESC.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DisableDepthDESC.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	DisableDepthDESC.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DisableDepthDESC.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	HR(md3dDevice->CreateDepthStencilState(&DisableDepthDESC, &md3dDisableDepthStencilState));

	//创建一个使得ZBuffer写无效的DSS
	D3D11_DEPTH_STENCIL_DESC DisableDepthWriteDESC;
	ZeroMemory(&DisableDepthWriteDESC, sizeof(DisableDepthWriteDESC));
	DisableDepthWriteDESC.DepthEnable = true;
	DisableDepthWriteDESC.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	DisableDepthWriteDESC.DepthFunc = D3D11_COMPARISON_LESS;
	DisableDepthWriteDESC.StencilEnable = true;
	DisableDepthWriteDESC.StencilReadMask = 0xff;
	DisableDepthWriteDESC.StencilWriteMask = 0xff;
	DisableDepthWriteDESC.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DisableDepthWriteDESC.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	DisableDepthWriteDESC.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	DisableDepthWriteDESC.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	DisableDepthWriteDESC.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DisableDepthWriteDESC.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	DisableDepthWriteDESC.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DisableDepthWriteDESC.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	HR(md3dDevice->CreateDepthStencilState(&DisableDepthWriteDESC, &md3dDisableZWriteDSS));


	//创建一个标记反射面的DepthStencilState状态
	D3D11_DEPTH_STENCIL_DESC MaskReflectDESC;
	ZeroMemory(&MaskReflectDESC, sizeof(MaskReflectDESC));
	MaskReflectDESC.DepthEnable = true;
	MaskReflectDESC.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	MaskReflectDESC.DepthFunc = D3D11_COMPARISON_LESS;
	MaskReflectDESC.StencilEnable = true;
	MaskReflectDESC.StencilReadMask = 0xff;
	MaskReflectDESC.StencilWriteMask = 0xff;
	MaskReflectDESC.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	MaskReflectDESC.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	MaskReflectDESC.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	MaskReflectDESC.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	MaskReflectDESC.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	MaskReflectDESC.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	MaskReflectDESC.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	MaskReflectDESC.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	HR(md3dDevice->CreateDepthStencilState(&MaskReflectDESC, &md3dDSSMaskReflect));


	//创建一个渲染反射面的DepthStencilState状态
	D3D11_DEPTH_STENCIL_DESC EnableReflectDESC;
	ZeroMemory(&EnableReflectDESC, sizeof(EnableReflectDESC));
	EnableReflectDESC.DepthEnable = false;
	EnableReflectDESC.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	EnableReflectDESC.DepthFunc = D3D11_COMPARISON_ALWAYS;
	EnableReflectDESC.StencilEnable = true;
	EnableReflectDESC.StencilReadMask = 0xff;
	EnableReflectDESC.StencilWriteMask = 0xff;
	EnableReflectDESC.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	EnableReflectDESC.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	EnableReflectDESC.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	EnableReflectDESC.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	EnableReflectDESC.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	EnableReflectDESC.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	EnableReflectDESC.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	EnableReflectDESC.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	HR(md3dDevice->CreateDepthStencilState(&EnableReflectDESC, &md3dDSSEnableReflect));


	//创建一个标记PointLightVolume的DSS
	D3D11_DEPTH_STENCIL_DESC MaskLightVolumeDESC;
	ZeroMemory(&MaskLightVolumeDESC, sizeof(MaskLightVolumeDESC));
	MaskLightVolumeDESC.DepthEnable = true;
	MaskLightVolumeDESC.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	MaskLightVolumeDESC.DepthFunc = D3D11_COMPARISON_LESS;
	MaskLightVolumeDESC.StencilEnable = true;
	MaskLightVolumeDESC.StencilReadMask = 0xff;
	MaskLightVolumeDESC.StencilWriteMask = 0xff;
	MaskLightVolumeDESC.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	MaskLightVolumeDESC.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	MaskLightVolumeDESC.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	MaskLightVolumeDESC.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	MaskLightVolumeDESC.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	MaskLightVolumeDESC.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	MaskLightVolumeDESC.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	MaskLightVolumeDESC.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	HR(md3dDevice->CreateDepthStencilState(&MaskLightVolumeDESC, &m_pDSSAddLightVolumeStencil));


	//创建一个渲染PointLightVolume的DSS
	D3D11_DEPTH_STENCIL_DESC RenderLightVolumeDESC;
	ZeroMemory(&RenderLightVolumeDESC, sizeof(RenderLightVolumeDESC));
	RenderLightVolumeDESC.DepthEnable = false;
	RenderLightVolumeDESC.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	RenderLightVolumeDESC.DepthFunc = D3D11_COMPARISON_ALWAYS;
	RenderLightVolumeDESC.StencilEnable = true;
	RenderLightVolumeDESC.StencilReadMask = 0xff;
	RenderLightVolumeDESC.StencilWriteMask = 0xff;
	RenderLightVolumeDESC.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	RenderLightVolumeDESC.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	RenderLightVolumeDESC.FrontFace.StencilPassOp = D3D11_STENCIL_OP_DECR;
	RenderLightVolumeDESC.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	RenderLightVolumeDESC.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	RenderLightVolumeDESC.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	RenderLightVolumeDESC.BackFace.StencilPassOp = D3D11_STENCIL_OP_DECR;
	RenderLightVolumeDESC.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	HR(md3dDevice->CreateDepthStencilState(&RenderLightVolumeDESC, &m_pDSSRenderLightVolume));

	//创建一个渲染SkyBox的DSS
	D3D11_DEPTH_STENCIL_DESC SkyBoxDESC;
	ZeroMemory(&SkyBoxDESC, sizeof(SkyBoxDESC));
	SkyBoxDESC.DepthEnable = true;
	SkyBoxDESC.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	SkyBoxDESC.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	SkyBoxDESC.StencilEnable = true;
	SkyBoxDESC.StencilReadMask = 0xff;
	SkyBoxDESC.StencilWriteMask = 0xff;
	//前面设定
	SkyBoxDESC.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	SkyBoxDESC.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	SkyBoxDESC.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	SkyBoxDESC.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	//背面设定,在光栅化状态剔除背面时这个设定没用,但是依然要设定,不然无法创建深度(模板)状态
	SkyBoxDESC.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	SkyBoxDESC.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	SkyBoxDESC.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	SkyBoxDESC.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	HR(md3dDevice->CreateDepthStencilState(&SkyBoxDESC, &renderSkyBoxDSS));

	//创建alpha混合开启的混合状态
	D3D11_BLEND_DESC blendStateDescription;
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;
	HR(md3dDevice->CreateBlendState(&blendStateDescription, &md3dEnableBlendState));

	//用于延迟渲染的光照混合的状态
	D3D11_BLEND_DESC lightBlendStateDesc;
	ZeroMemory(&lightBlendStateDesc, sizeof(D3D11_BLEND_DESC));
	lightBlendStateDesc.RenderTarget[0].BlendEnable = TRUE;
	lightBlendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	lightBlendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	lightBlendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	lightBlendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	lightBlendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	lightBlendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	lightBlendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HR(md3dDevice->CreateBlendState(&lightBlendStateDesc, &m_pLightBlendState));

	//创建alpha混合关闭的混合状态
	blendStateDescription.RenderTarget[0].BlendEnable = false;
	HR(md3dDevice->CreateBlendState(&blendStateDescription, &md3dDisableBlendState));

	//用于标记渲染阶段
	HR(md3dImmediateContext->QueryInterface(__uuidof(ID3DUserDefinedAnnotation), (void**)&d3dUserDefinedAnnot));

	return true;

}


void DirectxCore::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];

	//设置清除缓存(backbuffer),设置每帧初始颜色
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	//清除背后缓存
	md3dImmediateContext->ClearRenderTargetView(md3dRenderTargetView,color);

	//清除深度缓存和模板缓存，设置每帧初始值
	md3dImmediateContext->ClearDepthStencilView(md3dDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
}

void DirectxCore::EndScene()
{
	//因为渲染已经完成,提交backbuffer到屏幕
	if (mVsyncEnable)
	{
		//限帧提交
		md3dSwapChain->Present(1, 0);
	}
	else
	{
		//尽可能快提交
		md3dSwapChain->Present(0, 0);
	}
}



void DirectxCore::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, mVideoCardDescription);
	memory = mVideoCardMemory;
	return;
}


void DirectxCore::TurnOffAlphaBlend()
{
	float blendFactor[4];
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	md3dImmediateContext->OMSetBlendState(md3dDisableBlendState,blendFactor,1);
}


void DirectxCore::TurnOnAlphaBlend()
{
	float blendFactor[4];
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	md3dImmediateContext->OMSetBlendState(md3dEnableBlendState,blendFactor,1);
}


void DirectxCore::TurnOffZBuffer()
{
	md3dImmediateContext->OMSetDepthStencilState(md3dDisableDepthStencilState, 1);
}

void DirectxCore::SetBackBufferRender()
{
	md3dImmediateContext->OMSetRenderTargets(1, &md3dRenderTargetView, md3dDepthStencilView);
}


void DirectxCore::SetDefualtViewPort()
{
	md3dImmediateContext->RSSetViewports(1, &mViewport);
}

void DirectxCore::TurnOnZBuffer()
{
	md3dImmediateContext->OMSetDepthStencilState(md3dDepthStencilState, 0);
}
void DirectxCore::ShutDown()
{
	//在释放其它D3D接口前,使交换链进入窗口模式
	if (md3dSwapChain)
	{
		md3dSwapChain->SetFullscreenState(false, NULL);
	}

	ReleaseCOM(md3dRenderTargetView);
	ReleaseCOM(md3dDepthStencilView);
	ReleaseCOM(md3dDepthStencilBuffer);
	ReleaseCOM(md3dDepthStencilState);
	ReleaseCOM(md3dDisableDepthStencilState);
	ReleaseCOM(md3dEnableBlendState);
	ReleaseCOM(md3dDisableBlendState);
	ReleaseCOM(md3dRasterizerState);
	ReleaseCOM(md3dSwapChain);
	ReleaseCOM(md3dDevice);
	ReleaseCOM(md3dImmediateContext);
	ReleaseCOM(m_pTurnOffCullBackRS);
	ReleaseCOM(m_pDSSAddLightVolumeStencil);
	ReleaseCOM(m_pDSSRenderLightVolume);
}

void DirectxCore::RecoverDefualtRS()
{
	md3dImmediateContext->RSSetState(md3dRasterizerState);
}
void DirectxCore::TurnOnWireFrameRender()
{
	md3dImmediateContext->RSSetState(md3dWireFrameRS);
}

void DirectxCore::TurnOnMaskReflectDSS()
{
	md3dImmediateContext->OMSetDepthStencilState(md3dDSSMaskReflect, 1);
}

void DirectxCore::TurnOnEnableReflectDSS()
{
	md3dImmediateContext->OMSetDepthStencilState(md3dDSSEnableReflect, 1);
}

void DirectxCore::TurnOnCullFront()
{
	md3dImmediateContext->RSSetState(md3dCullFrontRS);
}


//恢复默认的
void DirectxCore::RecoverDefaultDSS()
{
	md3dImmediateContext->OMSetDepthStencilState(md3dDepthStencilState, 0);
}

//关掉ZBuffer写
void DirectxCore::TurnOnDisbleZWriteDSS()
{
	md3dImmediateContext->OMSetDepthStencilState(md3dDisableZWriteDSS, 0);
}

void DirectxCore::TurnOnLightBlend()
{
	float blendFactor[4];
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	md3dImmediateContext->OMSetBlendState(m_pLightBlendState, blendFactor, 1);
}

void DirectxCore::TurnOffFaceCull()
{
	md3dImmediateContext->RSSetState(m_pTurnOffCullBackRS);
}

void DirectxCore::TurnOnMaskLightVolumeDSS()
{
	md3dImmediateContext->OMSetDepthStencilState(m_pDSSAddLightVolumeStencil, 0);
}

void DirectxCore::TurnOnRenderLightVolumeDSS()
{
	md3dImmediateContext->OMSetDepthStencilState(m_pDSSRenderLightVolume, 1);
}

void DirectxCore::TurnOnRenderSkyBoxDSS()
{
	md3dImmediateContext->OMSetDepthStencilState(renderSkyBoxDSS, 0);
}


shared_ptr<DirectxCore> DirectxCore::m_pDirectxCore = nullptr;

