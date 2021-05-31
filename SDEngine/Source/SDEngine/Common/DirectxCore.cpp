#include "DirectxCore.h"
#include "../WindowInfo.h"


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


bool DirectxCore::Init(bool vsync, bool fullscreen, float ScreenDepth, float ScreenNear)
{
	
	//--------------------------------------------------------------
	//��ȡ��ʾģʽ��Ϣ���Կ���Ϣ
	//---------------------------------------------------------------

	IDXGIAdapter* adpter;//������
	IDXGIFactory* factory;
	unsigned int numModes, numerator, denominator, stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;

	//�洢vsyn�趨
	mVsyncEnable = vsync;

	//����һ��Directxͼ�νӿ�factory
	HR(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory));

	//ʹ��factory��Ϊ�Կ�����һ��adapter
	HR(factory->EnumAdapters(0, &adpter));

	//�о���Ҫ�����������
	HR(adpter->EnumOutputs(0, &mAdapterOutput));

	//��ȡ��Ӧ������DXGI_FORMAT_R8G8B8A8_UNORM��ʾ��ʽ��ģʽ��Ŀ
	HR(mAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL));

	//����һ����ʾģʽ�б���ſ��ܵ���ʾģʽ(�Կ�,������)
	displayModeList= new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
		return false;

	//�����ʾģʽ�б��ṹ��
	HR(mAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList));

	//������е���ʾģʽ,�ҵ��ʺ���Ļ���Ⱥ͸߶ȵ�
	//��һ��ģʽƥ��,�洢������ˢ���ٶȵķ��ӷ�ĸ??
	for (unsigned int i = 0; i<numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)GScreenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)GScreenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	//��ȡ������(�Կ�)����
	HR(adpter->GetDesc(&adapterDesc));

	//��ȡ�Կ��ڴ���
	mVideoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	//���Կ�����ת�����ַ�����
	/*error = wcstombs(&stringLength, mVideoCardDescription, 128, adapterDesc.Description, 128);

	if (error != 0)
	{
		return false;
	}*/

	//�ͷ���ʾģʽ�б�
	delete[] displayModeList;
	displayModeList = NULL;
	ReleaseCOM(adpter);
	ReleaseCOM(factory);


	//-----------------------------------------------------
	//��佻�������ݽṹ��
	//-----------------------------------------------------
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));

	sd.BufferDesc.Width = GScreenWidth;
	sd.BufferDesc.Height = GScreenHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	if (mVsyncEnable) //�޲���֡
	{
		sd.BufferDesc.RefreshRate.Numerator =numerator;
		sd.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		sd.BufferDesc.RefreshRate.Numerator = 0;
		sd.BufferDesc.RefreshRate.Denominator = 1;
	}
    
	//�رն��ز���
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	
	//�Ƿ����ȫ��
	if (fullscreen)
	{
		sd.Windowed = false;  
	}
	else
	{
		sd.Windowed = true;
	}
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;  //���󻺴�����
	sd.OutputWindow = GHwnd; //�����������Ĵ���
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	#if defined(_DEBUG)
	// If the project is in a debug build, enable the debug layer.
		sd.Flags = D3D11_CREATE_DEVICE_DEBUG;
	#endif
	
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//---------------------------------------------------------------
	//������������D3D�豸��D3D�豸������
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
	//����,�������󻺴���ͼ
	//--------------------------------------------------------------
	ID3D11Texture2D*backBuffer;
	md3dSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	md3dDevice->CreateRenderTargetView(backBuffer, 0, &md3dRenderTargetView);
	ReleaseCOM(backBuffer);

	//--------------------------------------------------------------
	//���2DTexture��Ȼ���(ģ�建��)���ݽṹ�壬������Ȼ���(ģ�建��)
	//--------------------------------------------------------------
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.Width = GScreenWidth;
	depthStencilDesc.Height = GScreenHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	HR(md3dDevice->CreateTexture2D(&depthStencilDesc,//Ҫ����������������
		0,
		&md3dDepthStencilBuffer)); //ָ����Ȼ����ָ��


	//-------------------------------------------------------------
	//�������趨��Ȼ���(ģ�建��)״̬��ָʾ���ʹ��Depth��stencil(Test)
	//-------------------------------------------------------------
	D3D11_DEPTH_STENCIL_DESC DSDESC;
	ZeroMemory(&DSDESC, sizeof(DSDESC));
	DSDESC.DepthEnable = true;
	DSDESC.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DSDESC.DepthFunc = D3D11_COMPARISON_LESS;
	DSDESC.StencilEnable = true;
	DSDESC.StencilReadMask = 0xff;
	DSDESC.StencilWriteMask = 0xff;

	//ǰ���趨
	DSDESC.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DSDESC.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	DSDESC.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	DSDESC.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	//�����趨,�ڹ�դ��״̬�޳�����ʱ����趨û��,������ȻҪ�趨,��Ȼ�޷��������(ģ��)״̬
	DSDESC.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DSDESC.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	DSDESC.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DSDESC.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	HR(md3dDevice->CreateDepthStencilState(&DSDESC, &md3dDepthStencilState));
	md3dImmediateContext->OMSetDepthStencilState(md3dDepthStencilState, 0);

	//--------------------------------------------------------------
	//������Ȼ���(ģ�建��)��ͼ
	//--------------------------------------------------------------
	HR(md3dDevice->CreateDepthStencilView(
		md3dDepthStencilBuffer, //���ǻ��������Ȼ���/©�ְ建�洴��һ����ͼ
		0,
		&md3dDepthStencilView));//ָ����Ȼ���/©�ְ���ͼ��ָ��

	//-------------------------------------------------------------
	//����Щ��ͼ�󶨵�����ϲ��׶�
	//-------------------------------------------------------------
	md3dImmediateContext->OMSetRenderTargets(1, &md3dRenderTargetView, md3dDepthStencilView);

	//�����Ĺ�դ��״̬
	HR(DirectXFrame::CreateRasterizerState(md3dDevice, &md3dRasterizerState));

	//�޳�ǰ��
	HR(DirectXFrame::CreateRasterizerState(md3dDevice, &md3dCullFrontRS, D3D11_CULL_FRONT));

	//�߿����
	HR(DirectXFrame::CreateRasterizerState(md3dDevice, &md3dWireFrameRS, D3D11_CULL_BACK, D3D11_FILL_WIREFRAME));

	//�������޳�
	HR(DirectXFrame::CreateRasterizerState(md3dDevice, &m_pTurnOffCullBackRS, D3D11_CULL_NONE));

	//-------------------------------------------------------------
	//�������趨�ӿ�
	//-------------------------------------------------------------
	mViewport.Width = static_cast<float>(GScreenWidth);
	mViewport.Height = static_cast<float>(GScreenHeight);
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;
	mViewport.TopLeftX = 0.0f;
	mViewport.TopLeftY = 0.0f;
	md3dImmediateContext->RSSetViewports(1,&mViewport);


	//��ʮ��,���һ��Text�ļ������Կ�����Ϣ 
	char CardInfo[128];
	int memory;
	GetVideoCardInfo(CardInfo, memory);
	ofstream os("I:/1.txt");
	os << "memory=" << memory << "  " << " CardInfo= " << CardInfo;


	//����һ��ʹZBuffer��Ч��DepthStencilState״̬
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

	//����һ��ʹ��ZBufferд��Ч��DSS
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

	//��������Depth, ����ZWrite��Ч��DSS
	D3D11_DEPTH_STENCIL_DESC equalDepthNoWriteDESC;
	ZeroMemory(&equalDepthNoWriteDESC, sizeof(equalDepthNoWriteDESC));
	equalDepthNoWriteDESC.DepthEnable = true;
	equalDepthNoWriteDESC.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	equalDepthNoWriteDESC.DepthFunc = D3D11_COMPARISON_EQUAL;
	equalDepthNoWriteDESC.StencilEnable = true;
	equalDepthNoWriteDESC.StencilReadMask = 0xff;
	equalDepthNoWriteDESC.StencilWriteMask = 0xff;
	equalDepthNoWriteDESC.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	equalDepthNoWriteDESC.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	equalDepthNoWriteDESC.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	equalDepthNoWriteDESC.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	equalDepthNoWriteDESC.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	equalDepthNoWriteDESC.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	equalDepthNoWriteDESC.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	equalDepthNoWriteDESC.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	HR(md3dDevice->CreateDepthStencilState(&equalDepthNoWriteDESC, &mEqualDepthStencilState));

	//����һ����Ƿ������DepthStencilState״̬
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


	//����һ����Ⱦ�������DepthStencilState״̬
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


	//����һ�����PointLightVolume��DSS
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


	//����һ����ȾPointLightVolume��DSS
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

	//����һ����ȾSkyBox��DSS
	D3D11_DEPTH_STENCIL_DESC SkyBoxDESC;
	ZeroMemory(&SkyBoxDESC, sizeof(SkyBoxDESC));
	SkyBoxDESC.DepthEnable = true;
	SkyBoxDESC.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	SkyBoxDESC.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	SkyBoxDESC.StencilEnable = true;
	SkyBoxDESC.StencilReadMask = 0xff;
	SkyBoxDESC.StencilWriteMask = 0xff;
	//ǰ���趨
	SkyBoxDESC.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	SkyBoxDESC.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	SkyBoxDESC.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	SkyBoxDESC.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	//�����趨,�ڹ�դ��״̬�޳�����ʱ����趨û��,������ȻҪ�趨,��Ȼ�޷��������(ģ��)״̬
	SkyBoxDESC.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	SkyBoxDESC.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	SkyBoxDESC.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	SkyBoxDESC.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	HR(md3dDevice->CreateDepthStencilState(&SkyBoxDESC, &renderSkyBoxDSS));

	//����alpha��Ͽ����Ļ��״̬
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

	//�����ӳ���Ⱦ�Ĺ��ջ�ϵ�״̬
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

	//����alpha��ϹرյĻ��״̬
	blendStateDescription.RenderTarget[0].BlendEnable = false;
	HR(md3dDevice->CreateBlendState(&blendStateDescription, &md3dDisableBlendState));

	//���ڱ����Ⱦ�׶�
	HR(md3dImmediateContext->QueryInterface(__uuidof(ID3DUserDefinedAnnotation), (void**)&d3dUserDefinedAnnot));

	return true;

}


void DirectxCore::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];

	//�����������(backbuffer),����ÿ֡��ʼ��ɫ
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	//������󻺴�
	md3dImmediateContext->ClearRenderTargetView(md3dRenderTargetView,color);

	//�����Ȼ����ģ�建�棬����ÿ֡��ʼֵ
	md3dImmediateContext->ClearDepthStencilView(md3dDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
}

void DirectxCore::EndScene()
{
	//��Ϊ��Ⱦ�Ѿ����,�ύbackbuffer����Ļ
	if (mVsyncEnable)
	{
		//��֡�ύ
		md3dSwapChain->Present(1, 0);
	}
	else
	{
		//�����ܿ��ύ
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
	//���ͷ�����D3D�ӿ�ǰ,ʹ���������봰��ģʽ
	if (md3dSwapChain)
	{
		md3dSwapChain->SetFullscreenState(false, NULL);
	}

	ReleaseCOM(md3dRenderTargetView);
	ReleaseCOM(md3dDepthStencilView);
	ReleaseCOM(md3dDepthStencilBuffer);
	ReleaseCOM(md3dDepthStencilState);
	ReleaseCOM(mEqualDepthStencilState);
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

//�ָ�Ĭ�ϵ�
void DirectxCore::RecoverDefaultDSS()
{
	md3dImmediateContext->OMSetDepthStencilState(md3dDepthStencilState, 0);
}

//�ص�ZBufferд
void DirectxCore::TurnOnDisbleZWriteDSS()
{
	md3dImmediateContext->OMSetDepthStencilState(md3dDisableZWriteDSS, 0);
}

void DirectxCore::TurnOnPreDepthDSS()
{
	md3dImmediateContext->OMSetDepthStencilState(mEqualDepthStencilState, 0);
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