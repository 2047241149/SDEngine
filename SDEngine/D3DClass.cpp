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
	//�ÿ�ָ��
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
	//��һ,��ȡ��ʾģʽ��Ϣ���Կ���Ϣ
	//---------------------------------------------------------------

	IDXGIAdapter* adpter;//������
	IDXGIFactory* factory;
	IDXGIOutput* adapterOutput;
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
	HR(adpter->EnumOutputs(0, &adapterOutput));

	//��ȡ��Ӧ������DXGI_FORMAT_R8G8B8A8_UNORM��ʾ��ʽ��ģʽ��Ŀ
	HR(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL));

	//����һ����ʾģʽ�б��ſ��ܵ���ʾģʽ(�Կ�,������)
	displayModeList= new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
		return false;

	//�����ʾģʽ�б�ṹ��
	HR(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList));

	//������е���ʾģʽ,�ҵ��ʺ���Ļ��Ⱥ͸߶ȵ�
	//��һ��ģʽƥ��,�洢������ˢ���ٶȵķ��ӷ�ĸ??
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

	//��ȡ������(�Կ�)����
	HR(adpter->GetDesc(&adapterDesc));

	//��ȡ�Կ��ڴ���
	mVideoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	//���Կ�����ת�����ַ�����
	error = wcstombs_s(&stringLength, mVideoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	//�ͷ���ʾģʽ�б�
	delete[] displayModeList;
	displayModeList = NULL;
	ReleaseCOM(adpter);
	ReleaseCOM(factory);


	//-----------------------------------------------------
	//�ڶ�,��佻�������ݽṹ��
	//-----------------------------------------------------
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));

	sd.BufferDesc.Width = ScreenWidth;
	sd.BufferDesc.Height = ScreenHeight;
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
	sd.OutputWindow = hwnd; //�����������Ĵ���
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;


	//---------------------------------------------------------------
	//����,������������D3D�豸��D3D�豸������
	//---------------------------------------------------------------
	D3D_FEATURE_LEVEL featureLevel;
	featureLevel = D3D_FEATURE_LEVEL_11_0;
	HR(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &sd, &md3dSwapChain, &md3dDevice, NULL, &md3dImmediateContext));


	//--------------------------------------------------------------
	//����,�������󻺴���ͼ
	//--------------------------------------------------------------
	ID3D11Texture2D*backBuffer;
	md3dSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	md3dDevice->CreateRenderTargetView(backBuffer, 0, &md3dRenderTargetView);
	ReleaseCOM(backBuffer);


	//--------------------------------------------------------------
	//����,���2DTexture��Ȼ���(ģ�建��)���ݽṹ�壬������Ȼ���(ģ�建��)
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
	HR(md3dDevice->CreateTexture2D(&depthStencilDesc,//Ҫ���������������
		0,
		&md3dDepthStencilBuffer)); //ָ����Ȼ����ָ��


	//-------------------------------------------------------------
	//����,�������趨��Ȼ���(ģ�建��)״̬��ָʾ���ʹ��Depth��stencil(Test)
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
	DSDESC.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	DSDESC.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DSDESC.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	//�����趨,�ڹ�դ��״̬�޳�����ʱ����趨û��,������ȻҪ�趨,��Ȼ�޷��������(ģ��)״̬
	DSDESC.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DSDESC.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	DSDESC.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DSDESC.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	HR(md3dDevice->CreateDepthStencilState(&DSDESC, &md3dDepthStencilState));
	md3dImmediateContext->OMSetDepthStencilState(md3dDepthStencilState, 1);

	//--------------------------------------------------------------
	//����,������Ȼ���(ģ�建��)��ͼ
	//--------------------------------------------------------------

	HR(md3dDevice->CreateDepthStencilView(
		md3dDepthStencilBuffer, //���ǻ��������Ȼ���/©�ְ建�洴��һ����ͼ
		0,
		&md3dDepthStencilView));//ָ����Ȼ���/©�ְ���ͼ��ָ��


	//-------------------------------------------------------------
	//�ڰ�,����Щ��ͼ�󶨵�����ϲ��׶�
	//-------------------------------------------------------------
	md3dImmediateContext->OMSetRenderTargets(1, &md3dRenderTargetView, md3dDepthStencilView);


	//-------------------------------------------------------------
	//�ھ�,�������趨��դ��״̬,���ڿ��������ȾĿ��(���߿���ʵ��ģʽ�ȵ�)
	//-------------------------------------------------------------
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK; //�����޳�
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true; //��Ȳü�����
	rasterDesc.FillMode = D3D11_FILL_SOLID; //ʵ����Ⱦ
	rasterDesc.FrontCounterClockwise = false; //˳ʱ��
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	HR(md3dDevice->CreateRasterizerState(&rasterDesc, &md3dRasterizerState));
	md3dImmediateContext->RSSetState(md3dRasterizerState);


	rasterDesc.FillMode = D3D11_FILL_WIREFRAME; 

	HR(md3dDevice->CreateRasterizerState(&rasterDesc, &md3dWireFrameRS));
	

	//-------------------------------------------------------------
	//��ʮ,�������趨�ӿ�
	//-------------------------------------------------------------
	mViewport.Width = static_cast<float>(ScreenWidth);
	mViewport.Height = static_cast<float>(ScreenHeight);
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



	//��ʮ��,����һ��ʹZBuffer��Ч��DepthStencilState״̬
	D3D11_DEPTH_STENCIL_DESC DisableDepthDESC;
	ZeroMemory(&DisableDepthDESC, sizeof(DisableDepthDESC));
	DisableDepthDESC.DepthEnable = false;
	DisableDepthDESC.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DisableDepthDESC.DepthFunc = D3D11_COMPARISON_LESS;
	DisableDepthDESC.StencilEnable = true;
	DisableDepthDESC.StencilReadMask = 0xff;
	DisableDepthDESC.StencilWriteMask = 0xff;
	//ǰ���趨
	DisableDepthDESC.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DisableDepthDESC.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	DisableDepthDESC.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DisableDepthDESC.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	//�����趨,�ڹ�դ��״̬�޳�����ʱ����趨û��,������ȻҪ�趨,��Ȼ�޷��������(ģ��)״̬
	DisableDepthDESC.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DisableDepthDESC.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	DisableDepthDESC.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DisableDepthDESC.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	HR(md3dDevice->CreateDepthStencilState(&DisableDepthDESC, &md3dDisableDepthStencilState));

	//��ʮ��,����alpha��Ͽ����Ļ��״̬
	D3D11_BLEND_DESC blendStateDescription;

	//��ʼ��������ݽṹ��
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

	//��ʮ��,����alpha��ϹرյĻ��״̬
	blendStateDescription.RenderTarget[0].BlendEnable = false;
	HR(md3dDevice->CreateBlendState(&blendStateDescription, &md3dDisableBlendState));


	return true;

}


void D3DClass::BeginScene(float red, float green, float blue, float alpha)
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

void D3DClass::EndScene()
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
	//���ͷ�����D3D�ӿ�ǰ,ʹ���������봰��ģʽ
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
