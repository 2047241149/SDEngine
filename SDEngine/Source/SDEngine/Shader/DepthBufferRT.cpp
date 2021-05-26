#include "DepthBufferRT.h"

DepthBufferRT::DepthBufferRT(int TextureWidth, int TextureHeight)
{
	mShaderResourceView = NULL;
	mDepthStencilTexture = NULL;
	mDepthStencilView = NULL;
	
	Initialize(TextureWidth, TextureHeight);
}


DepthBufferRT::DepthBufferRT(const DepthBufferRT&other)
{

}

DepthBufferRT::~DepthBufferRT()
{
	ShutDown();
}


bool DepthBufferRT::Initialize(int TextureWidth, int TextureHeight)
{

	//第一,填充深度视图的2D纹理形容结构体,并创建2D渲染纹理
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = TextureWidth;
	depthBufferDesc.Height = TextureHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_R24G8_TYPELESS; //24位是为了深度缓存，8位是为了模板缓存
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;  //注意深度缓存(纹理)的绑定标志
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;
	HR(g_pDevice->CreateTexture2D(&depthBufferDesc, NULL, &mDepthStencilTexture));
	

	//第二,填充深度缓存视图形容结构体,并创建深度缓存视图
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	HR(g_pDevice->CreateDepthStencilView(mDepthStencilTexture, &depthStencilViewDesc, &mDepthStencilView));


	//第三,填充着色器资源视图形容体,并进行创建着色器资源视图,注意这是用深度缓存(纹理)来创建的，而不是渲染目标缓存(纹理)创建的
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; //此时因为是仅仅进行深度写，而不是颜色写，所以此时Shader资源格式跟深度缓存是一样的
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = depthBufferDesc.MipLevels;
	HR(g_pDevice->CreateShaderResourceView(mDepthStencilTexture, &shaderResourceViewDesc, &mShaderResourceView));
    
	//第四，设置视口的属性
	mViewPort.Width = (float)TextureWidth;
	mViewPort.Height = (float)TextureHeight;
	mViewPort.MinDepth = 0.0f;
	mViewPort.MaxDepth = 1.0f;
	mViewPort.TopLeftX = 0.0f;
	mViewPort.TopLeftY = 0.0f;

	return true;

}


void DepthBufferRT::ShutDown()
{
	ReleaseCOM(mDepthStencilTexture);
	ReleaseCOM(mDepthStencilView);
	ReleaseCOM(mShaderResourceView);
}


//让此时所有图形渲染到这个目前渲染的位置
void DepthBufferRT::SetRenderTarget()
{
	this->ClearDepth();

	ID3D11RenderTargetView* renderTarget[1] = { nullptr };

	//绑定渲染目标视图和深度模板视图到输出渲染管线
	g_pDeviceContext->OMSetRenderTargets(1,renderTarget, mDepthStencilView);

	//设置视口
	g_pDeviceContext->RSSetViewports(1, &mViewPort);
}


//不用清除背后缓存,因为不需要进颜色写(ColorWrite),仅仅进行深度写
void DepthBufferRT::ClearDepth()
{

	//清除深度缓存和模板缓存
	g_pDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

// 将“被渲染模型到纹理的纹理”作为ShaderResourceView资源返回，这个资源将会跟其它的ShaderResourceView资源一样被送入Shader里计算.
ID3D11ShaderResourceView* DepthBufferRT::GetShaderResourceView()
{
	return mShaderResourceView;
}


