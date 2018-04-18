#include"D3DClass.h"

D3DClass::D3DClass()
{
	mD3D = nullptr;
}

D3DClass::~D3DClass()
{
	
}


D3DClass::D3DClass(const D3DClass& other)
{
	ShutDown();
}

D3DClass* D3DClass::GetInstance()
{
	if (mD3D == nullptr)
	{
		mD3D = shared_ptr<D3DClass>(new D3DClass());
	}
	return mD3D.get();
}


bool D3DClass::Initialize(int ScreenWidth, int ScreenHeight, bool vsync, HWND hwnd, bool fullscreen, float ScreenDepth, float ScreenNear)
{
	//置空指针
	md3dDevice = NULL;
	md3dImmediateContext = NULL;
	md3dSwapChain = NULL;
	md3dRenderTargetView = NULL;
	md3dDepthStencilView = NULL;
	md3dDepthStencilBuffer = NULL;
	md3dDepthStencilState = NULL;
	md3dDisableDepthStencilState = NULL;
	md3dRasterizerState = NULL;
	md3dEnableBlendState = NULL;
	md3dDisableBlendState = NULL;
	md3dWireFrameRS = NULL;

	//--------------------------------------------------------------
	//第一,获取显示模式信息和显卡信息
	//---------------------------------------------------------------

	IDXGIAdapter* adpter;//适配器
	IDXGIFactory* factory;
	IDXGIOutput* adapterOutput;
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
	HR(adpter->EnumOutputs(0, &adapterOutput));

	//获取适应适配器DXGI_FORMAT_R8G8B8A8_UNORM显示格式的模式数目
	HR(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL));

	//创建一个显示模式列表存放可能的显示模式(显卡,监视器)
	displayModeList= new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
		return false;

	//填充显示模式列表结构体
	HR(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList));

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
	//第二,填充交换链形容结构体
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
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;


	//---------------------------------------------------------------
	//第三,创建交换链和D3D设备和D3D设备上下文
	//---------------------------------------------------------------
	D3D_FEATURE_LEVEL featureLevel;
	featureLevel = D3D_FEATURE_LEVEL_11_0;
	HR(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &sd, &md3dSwapChain, &md3dDevice, NULL, &md3dImmediateContext));


	//--------------------------------------------------------------
	//第四,创建背后缓存视图
	//--------------------------------------------------------------
	ID3D11Texture2D*backBuffer;
	md3dSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	md3dDevice->CreateRenderTargetView(backBuffer, 0, &md3dRenderTargetView);
	ReleaseCOM(backBuffer);


	//--------------------------------------------------------------
	//第五,填充2DTexture深度缓存(模板缓存)形容结构体，创建深度缓存(模板缓存)
	//--------------------------------------------------------------
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.Width=ScreenWidth;
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
	//第六,创建并设定深度缓存(模板缓存)状态，指示如何使用Depth和stencil(Test)
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
	DSDESC.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	DSDESC.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DSDESC.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	//背面设定,在光栅化状态剔除背面时这个设定没用,但是依然要设定,不然无法创建深度(模板)状态
	DSDESC.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DSDESC.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	DSDESC.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DSDESC.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	HR(md3dDevice->CreateDepthStencilState(&DSDESC, &md3dDepthStencilState));
	md3dImmediateContext->OMSetDepthStencilState(md3dDepthStencilState, 1);

	//--------------------------------------------------------------
	//第七,创建深度缓存(模板缓存)视图
	//--------------------------------------------------------------

	HR(md3dDevice->CreateDepthStencilView(
		md3dDepthStencilBuffer, //我们基于这个深度缓存/漏字板缓存创建一个视图
		0,
		&md3dDepthStencilView));//指向深度缓存/漏字板视图的指针


	//-------------------------------------------------------------
	//第八,把那些视图绑定到输出合并阶段
	//-------------------------------------------------------------
	md3dImmediateContext->OMSetRenderTargets(1, &md3dRenderTargetView, md3dDepthStencilView);


	//-------------------------------------------------------------
	//第九,创建并设定光栅化状态,用于控制如何渲染目标(以线框还是实体模式等等)
	//-------------------------------------------------------------
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK; //背面剔除
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true; //深度裁剪开启
	rasterDesc.FillMode = D3D11_FILL_SOLID; //实体渲染
	rasterDesc.FrontCounterClockwise = false; //顺时针
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	HR(md3dDevice->CreateRasterizerState(&rasterDesc, &md3dRasterizerState));
	md3dImmediateContext->RSSetState(md3dRasterizerState);


	rasterDesc.FillMode = D3D11_FILL_WIREFRAME; 

	HR(md3dDevice->CreateRasterizerState(&rasterDesc, &md3dWireFrameRS));
	

	//-------------------------------------------------------------
	//第十,创建并设定视口
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



	//第十三,创建一个使ZBuffer无效的DepthStencilState状态
	D3D11_DEPTH_STENCIL_DESC DisableDepthDESC;
	ZeroMemory(&DisableDepthDESC, sizeof(DisableDepthDESC));
	DisableDepthDESC.DepthEnable = false;
	DisableDepthDESC.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DisableDepthDESC.DepthFunc = D3D11_COMPARISON_LESS;
	DisableDepthDESC.StencilEnable = true;
	DisableDepthDESC.StencilReadMask = 0xff;
	DisableDepthDESC.StencilWriteMask = 0xff;
	//前面设定
	DisableDepthDESC.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DisableDepthDESC.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	DisableDepthDESC.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DisableDepthDESC.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	//背面设定,在光栅化状态剔除背面时这个设定没用,但是依然要设定,不然无法创建深度(模板)状态
	DisableDepthDESC.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DisableDepthDESC.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	DisableDepthDESC.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DisableDepthDESC.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	HR(md3dDevice->CreateDepthStencilState(&DisableDepthDESC, &md3dDisableDepthStencilState));

	//第十四,创建alpha混合开启的混合状态
	D3D11_BLEND_DESC blendStateDescription;

	//初始化混合形容结构体
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;
	HR(md3dDevice->CreateBlendState(&blendStateDescription, &md3dEnableBlendState));

	//第十五,创建alpha混合关闭的混合状态
	blendStateDescription.RenderTarget[0].BlendEnable = false;
	HR(md3dDevice->CreateBlendState(&blendStateDescription, &md3dDisableBlendState));


	return true;

}


void D3DClass::BeginScene(float red, float green, float blue, float alpha)
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

void D3DClass::EndScene()
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



void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, mVideoCardDescription);
	memory = mVideoCardMemory;
	return;
}


void D3DClass::TurnOffAlphaBlend()
{
	float blendFactor[4];
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	md3dImmediateContext->OMSetBlendState(md3dDisableBlendState,blendFactor,1);
}


void D3DClass::TurnOnAlphaBlend()
{
	float blendFactor[4];
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	md3dImmediateContext->OMSetBlendState(md3dEnableBlendState,blendFactor,1);
}


void D3DClass::TurnOffZBuffer()
{
	md3dImmediateContext->OMSetDepthStencilState(md3dDisableDepthStencilState, 1);
}

void D3DClass::SetBackBufferRender()
{
	md3dImmediateContext->OMSetRenderTargets(1, &md3dRenderTargetView, md3dDepthStencilView);
}


void D3DClass::SetViewPort()
{
	md3dImmediateContext->RSSetViewports(1, &mViewport);
}

void D3DClass::TurnOnZBuffer()
{
	md3dImmediateContext->OMSetDepthStencilState(md3dDepthStencilState, 1);
}
void D3DClass::ShutDown()
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
}

void D3DClass::TurnOnSolidRender()
{
	md3dImmediateContext->RSSetState(md3dRasterizerState);
}
void D3DClass::TurnOnWireFrameRender()
{
	md3dImmediateContext->RSSetState(md3dWireFrameRS);
}

shared_ptr<D3DClass> D3DClass::mD3D = nullptr;
