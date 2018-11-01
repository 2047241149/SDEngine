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

	//��һ,���2D�������ݽṹ��,������2D��ȾĿ������

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
	
	

	//�ڶ��������ȾĿ����ͼ������,�����д���Ŀ����Ⱦ��ͼ
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	HR(g_pDevice->CreateRenderTargetView(m_pBackTexture2D, &renderTargetViewDesc, &m_pRTV));
	

	//����,������ɫ����Դ��ͼ
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	HR(g_pDevice->CreateShaderResourceView(m_pBackTexture2D, &shaderResourceViewDesc, &m_pSRV));
	

	
	//����,������Ȼ���(ģ�建��)
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
	HR(g_pDevice->CreateTexture2D(&depthStencilDesc,//Ҫ���������������
		0,
		&m_pDepthTexture2D));


	//����,������Ȼ���(ģ�建��)��ͼ
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	HR(g_pDevice->CreateDepthStencilView(
		m_pDepthTexture2D, //���ǻ��������Ȼ���/©�ְ建�洴��һ����ͼ
		&depthStencilViewDesc,
		&m_pDSV));//ָ����Ȼ���/©�ְ���ͼ��ָ��


	//����,������Ⱦ���ӿ�
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


//�ô�ʱ����ͼ����Ⱦ�����Ŀǰ��Ⱦ��λ��
void RenderTexture::SetRenderTarget(float fRed, float fGreen, float fBlue, float fAlpha)
{
	//����ȾĿ����ͼ�����ģ����ͼ�������Ⱦ���ߣ���ʱ��Ⱦ���������������
	g_pDeviceContext->OMSetRenderTargets(1, &m_pRTV, m_pDSV);

	//������Ӧ���ӿ�
	g_pDeviceContext->RSSetViewports(1, &m_ViewPort);

	ClearRenderTarget(fRed, fGreen, fBlue, fAlpha);
	ClearDepthBuffer();
}


void RenderTexture::ClearDepthBuffer()
{
	//�����Ȼ����ģ�建��
	g_pDeviceContext->ClearDepthStencilView(m_pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}



void RenderTexture::ClearRenderTarget(float red, float green, float blue, float alpha)
{
	//�����������Ϊ����ɫ
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