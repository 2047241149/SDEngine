#include "CascadeShadowMap.h"


CascadeShadowMap::CascadeShadowMap(int nTextureWidth, int nTexureHeight, int nCascadeNum):
	m_nTextureWidth(nTextureWidth),
	m_nTextureHeight(nTexureHeight),
	m_pDSV(nullptr),
	m_pDepthTexture2D(nullptr),
	m_pShadowMap(nullptr)
{

	Init(nTextureWidth, nTexureHeight, nCascadeNum);
}


CascadeShadowMap::CascadeShadowMap(const CascadeShadowMap&other)
{

}

CascadeShadowMap::~CascadeShadowMap()
{
	ShutDown();
}

bool CascadeShadowMap::Init(int nTextureWidth, int nTexureHeight, int nCascadeNum)
{

	//������Ȼ���(ģ�建��)
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.Width = nTextureWidth * 3;
	depthStencilDesc.Height = nTexureHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	HR(g_pDevice->CreateTexture2D(&depthStencilDesc,//Ҫ����������������
		0,
		&m_pDepthTexture2D));


	//����,������Ȼ���(ģ�建��)��ͼ
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	HR(g_pDevice->CreateDepthStencilView(
		m_pDepthTexture2D,
		&depthStencilViewDesc,
		&m_pDSV));


	//������ɫ����Դ��ͼ
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = depthStencilDesc.MipLevels;

	HR(g_pDevice->CreateShaderResourceView(m_pDepthTexture2D, &shaderResourceViewDesc, &m_pShadowMap));

	return true;

}


void CascadeShadowMap::ShutDown()
{
	ReleaseCOM(m_pShadowMap);
	ReleaseCOM(m_pDepthTexture2D);
	ReleaseCOM(m_pDSV);
}


//�ô�ʱ����ͼ����Ⱦ�����Ŀǰ��Ⱦ��λ��
void CascadeShadowMap::SetRenderTarget(int nCascadeIndex)
{
	//����ȾĿ����ͼ�����ģ����ͼ�������Ⱦ���ߣ���ʱ��Ⱦ���������������
	ID3D11RenderTargetView* rtv[1] = { nullptr };
	g_pDeviceContext->OMSetRenderTargets(1, rtv, m_pDSV);
//	ClearDepthBuffer();

	//������Ⱦ���ӿ�
	m_ViewPort.Width = static_cast<float>(m_nTextureWidth);
	m_ViewPort.Height = static_cast<float>(m_nTextureHeight);
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;
	m_ViewPort.TopLeftX = (float)m_nTextureWidth * (float)nCascadeIndex;
	m_ViewPort.TopLeftY = 0.0f;
	//������Ӧ���ӿ�
	g_pDeviceContext->RSSetViewports(1, &m_ViewPort);

	
}


void CascadeShadowMap::ClearDepthBuffer()
{
	//�����Ȼ����ģ�建��
	g_pDeviceContext->ClearDepthStencilView(m_pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}



ID3D11ShaderResourceView* CascadeShadowMap::GetShadowMap()
{
	return m_pShadowMap;
}


ID3D11DepthStencilView* CascadeShadowMap::GetDSV()
{
	return m_pDSV;
}