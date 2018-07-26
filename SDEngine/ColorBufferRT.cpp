#include "ColorBufferRT.h"

ColorBufferRT::ColorBufferRT(int TextureWidth, int TexureHeight, float ScreenDepth, float ScreenNear):
	m_nTextureWidth(TextureWidth),
	m_nTextureHeight(TexureHeight)
{
	mRenderTargetTexture = nullptr;
	mDepthStencilBuffer = NULL;
	mRenderTargetView = NULL;
	mDepthStencilBuffer = NULL;
	mDepthStencilView = NULL;

	Initialize(TextureWidth, TexureHeight, ScreenDepth, ScreenNear);
}


ColorBufferRT::ColorBufferRT(const ColorBufferRT&other)
{

}

ColorBufferRT::~ColorBufferRT()
{

}


bool ColorBufferRT::Initialize(int TextureWidth, int TexureHeight, float ScreenDepth, float ScreenNear)
{

	
	//��һ,���2D�������ݽṹ��,������2D��ȾĿ������
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	textureDesc.Width = TextureWidth;
	textureDesc.Height = TexureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;  //��������Ϊ12���ֽ�
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0; 
	textureDesc.MiscFlags = 0;                    


	ID3D11Device* d3dDevice = D3DClass::GetInstance()->GetDevice();

	HR(g_pDevice->CreateTexture2D(&textureDesc, NULL, &mRenderTargetTexture));
	
	

	//�ڶ��������ȾĿ����ͼ������,�����д���Ŀ����Ⱦ��ͼ
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;


	HR(g_pDevice->CreateRenderTargetView(mRenderTargetTexture, &renderTargetViewDesc, &mRenderTargetView));
	

	//����,�����ɫ����Դ��ͼ������,�����д�����ɫ����Դ��ͼ
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	HR(g_pDevice->CreateShaderResourceView(mRenderTargetTexture, &shaderResourceViewDesc, &mShaderResourceView));
	

	
	//����,���2DTexture��Ȼ���(ģ�建��)���ݽṹ�壬������Ȼ���(ģ�建��)
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.Width = TextureWidth;
	depthStencilDesc.Height = TexureHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	HR(g_pDevice->CreateTexture2D(&depthStencilDesc,//Ҫ���������������
		0,
		&mDepthStencilBuffer)); //ָ����Ȼ����ָ��


	//����,�����Ȼ�����ͼ���ݽṹ��,������Ȼ���(ģ�建��)��ͼ
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	HR(g_pDevice->CreateDepthStencilView(
		mDepthStencilBuffer, //���ǻ��������Ȼ���/©�ְ建�洴��һ����ͼ
		&depthStencilViewDesc,
		&mDepthStencilView));//ָ����Ȼ���/©�ְ���ͼ��ָ��


	//����,������Ⱦ���ӿ�
	md3dViewport.Width = static_cast<float>(TextureWidth);
	md3dViewport.Height = static_cast<float>(TexureHeight);
	md3dViewport.MinDepth = 0.0f;
	md3dViewport.MaxDepth = 1.0f;
	md3dViewport.TopLeftX = 0.0f;
	md3dViewport.TopLeftY = 0.0f;

	return true;

}


void ColorBufferRT::ShutDown()
{

	ReleaseCOM(mShaderResourceView);
	ReleaseCOM(mRenderTargetTexture);
	ReleaseCOM(mRenderTargetView);
	
	ReleaseCOM(mDepthStencilBuffer);
	ReleaseCOM(mDepthStencilView);
}


//�ô�ʱ����ͼ����Ⱦ�����Ŀǰ��Ⱦ��λ��
void ColorBufferRT::SetRenderTarget()
{
	ID3D11DeviceContext* deviceContext = D3DClass::GetInstance()->GetDeviceContext();
	//����ȾĿ����ͼ�����ģ����ͼ�������Ⱦ���ߣ���ʱ��Ⱦ���������������
	deviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

	//������Ӧ���ӿ�
	deviceContext->RSSetViewports(1, &md3dViewport);

	ClearRenderTarget(0.0, 0.0, 0.0, 1.0f);
	ClearDepthBuffer();
}


void ColorBufferRT::ClearDepthBuffer()
{
	ID3D11DeviceContext* deviceContext = D3DClass::GetInstance()->GetDeviceContext();

	//�����Ȼ����ģ�建��
	deviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}



void ColorBufferRT::ClearRenderTarget(float red, float green, float blue, float alpha)
{
	//�����������Ϊ����ɫ
	float color[4];
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	ID3D11DeviceContext* deviceContext = D3DClass::GetInstance()->GetDeviceContext();
	deviceContext->ClearRenderTargetView(mRenderTargetView, color);

}


// ��������Ⱦģ�͵������������ΪShaderResourceView��Դ���أ������Դ�����������ShaderResourceView��Դһ��������Shader�����.
ID3D11ShaderResourceView* ColorBufferRT::GetShaderResourceView()
{
	return mShaderResourceView;
}
