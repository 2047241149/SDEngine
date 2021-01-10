#include "RenderTexture.h"

RenderTexture::RenderTexture(int nTextureWidth, int nTexureHeight, TextureFormat eTextureFormat):
	m_nTextureWidth(nTextureWidth),
	m_nTextureHeight(nTexureHeight),
	m_pRTV(nullptr),
	m_pDSV(nullptr),
	m_pDepthTexture2D(nullptr),
	m_pSRV(nullptr),
	m_pBackTexture2D(nullptr)
{

	Init(nTextureWidth, nTexureHeight);
}


RenderTexture::RenderTexture(const RenderTexture&other)
{

}

RenderTexture::~RenderTexture()
{
	ShutDown();
}


bool RenderTexture::Init(int nTextureWidth, int nTexureHeight, TextureFormat eTextureFormat)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	switch (eTextureFormat)
	{
	case R32G32B32A32:
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	
		break;
	case R32:
		textureDesc.Format = DXGI_FORMAT_R32_FLOAT;
		break;
	default:
		return false;
	}

	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	renderTargetViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.Format = textureDesc.Format;
	depthStencilViewDesc.Format = depthStencilDesc.Format;

	//第一,填充2D纹理形容结构体,并创建2D渲染目标纹理

	textureDesc.Width = nTextureWidth;
	textureDesc.Height = nTexureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0; 
	textureDesc.MiscFlags = 0;                    
	HR(g_pDevice->CreateTexture2D(&textureDesc, NULL, &m_pBackTexture2D));
	
	//第二，填充渲染目标视图形容体,并进行创建目标渲染视图
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	HR(g_pDevice->CreateRenderTargetView(m_pBackTexture2D, &renderTargetViewDesc, &m_pRTV));
	
	//第三,创建着色器资源视图
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	HR(g_pDevice->CreateShaderResourceView(m_pBackTexture2D, &shaderResourceViewDesc, &m_pSRV));
	

	//第四,创建深度缓存(模板缓存)
	depthStencilDesc.Width = nTextureWidth;
	depthStencilDesc.Height = nTexureHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	HR(g_pDevice->CreateTexture2D(&depthStencilDesc,//要创建的纹理的形容
		0,
		&m_pDepthTexture2D));

	//第五,创建深度缓存(模板缓存)视图
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	HR(g_pDevice->CreateDepthStencilView(
		m_pDepthTexture2D, //我们基于这个深度缓存/漏字板缓存创建一个视图
		&depthStencilViewDesc,
		&m_pDSV));//指向深度缓存/漏字板视图的指针

	//第六,设置渲染的视口
	m_ViewPort.Width = static_cast<float>(nTextureWidth);
	m_ViewPort.Height = static_cast<float>(nTexureHeight);
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;
	m_ViewPort.TopLeftX = 0.0f;
	m_ViewPort.TopLeftY = 0.0f;
	return true;
}


void RenderTexture::ShutDown()
{
	ReleaseCOM(m_pSRV);
	ReleaseCOM(m_pBackTexture2D);
	ReleaseCOM(m_pRTV);
	ReleaseCOM(m_pDepthTexture2D);
	ReleaseCOM(m_pDSV);
}


//让此时所有图形渲染到这个目前渲染的位置
void RenderTexture::SetRenderTarget(float fRed, float fGreen, float fBlue, float fAlpha)
{
	//绑定渲染目标视图和深度模板视图到输出渲染管线，此时渲染输出到两张纹理中
	g_pDeviceContext->OMSetRenderTargets(1, &m_pRTV, m_pDSV);

	//设置相应的视口
	g_pDeviceContext->RSSetViewports(1, &m_ViewPort);

	ClearRenderTarget(fRed, fGreen, fBlue, fAlpha);
	ClearDepthBuffer();
}


void RenderTexture::ClearDepthBuffer()
{
	//清除深度缓存和模板缓存
	g_pDeviceContext->ClearDepthStencilView(m_pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}



void RenderTexture::ClearRenderTarget(float red, float green, float blue, float alpha)
{
	//设置清除缓存为的颜色
	float color[4];
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	g_pDeviceContext->ClearRenderTargetView(m_pRTV, color);

}

ID3D11ShaderResourceView* RenderTexture::GetSRV()
{
	return m_pSRV;
}


ID3D11RenderTargetView* RenderTexture::GetRenderTargetView()
{
	return m_pRTV;
}