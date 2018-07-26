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

	
	//第一,填充2D纹理形容结构体,并创建2D渲染目标纹理
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	textureDesc.Width = TextureWidth;
	textureDesc.Height = TexureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;  //纹理像素为12个字节
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0; 
	textureDesc.MiscFlags = 0;                    


	ID3D11Device* d3dDevice = D3DClass::GetInstance()->GetDevice();

	HR(g_pDevice->CreateTexture2D(&textureDesc, NULL, &mRenderTargetTexture));
	
	

	//第二，填充渲染目标视图形容体,并进行创建目标渲染视图
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;


	HR(g_pDevice->CreateRenderTargetView(mRenderTargetTexture, &renderTargetViewDesc, &mRenderTargetView));
	

	//第三,填充着色器资源视图形容体,并进行创建着色器资源视图
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	HR(g_pDevice->CreateShaderResourceView(mRenderTargetTexture, &shaderResourceViewDesc, &mShaderResourceView));
	

	
	//第四,填充2DTexture深度缓存(模板缓存)形容结构体，创建深度缓存(模板缓存)
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
	HR(g_pDevice->CreateTexture2D(&depthStencilDesc,//要创建的纹理的形容
		0,
		&mDepthStencilBuffer)); //指向深度缓存的指针


	//第五,填充深度缓存视图形容结构体,创建深度缓存(模板缓存)视图
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	HR(g_pDevice->CreateDepthStencilView(
		mDepthStencilBuffer, //我们基于这个深度缓存/漏字板缓存创建一个视图
		&depthStencilViewDesc,
		&mDepthStencilView));//指向深度缓存/漏字板视图的指针


	//第六,设置渲染的视口
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


//让此时所有图形渲染到这个目前渲染的位置
void ColorBufferRT::SetRenderTarget()
{
	ID3D11DeviceContext* deviceContext = D3DClass::GetInstance()->GetDeviceContext();
	//绑定渲染目标视图和深度模板视图到输出渲染管线，此时渲染输出到两张纹理中
	deviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

	//设置相应的视口
	deviceContext->RSSetViewports(1, &md3dViewport);

	ClearRenderTarget(0.0, 0.0, 0.0, 1.0f);
	ClearDepthBuffer();
}


void ColorBufferRT::ClearDepthBuffer()
{
	ID3D11DeviceContext* deviceContext = D3DClass::GetInstance()->GetDeviceContext();

	//清除深度缓存和模板缓存
	deviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}



void ColorBufferRT::ClearRenderTarget(float red, float green, float blue, float alpha)
{
	//设置清除缓存为的颜色
	float color[4];
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	ID3D11DeviceContext* deviceContext = D3DClass::GetInstance()->GetDeviceContext();
	deviceContext->ClearRenderTargetView(mRenderTargetView, color);

}


// 将“被渲染模型到纹理的纹理”作为ShaderResourceView资源返回，这个资源将会跟其它的ShaderResourceView资源一样被送入Shader里计算.
ID3D11ShaderResourceView* ColorBufferRT::GetShaderResourceView()
{
	return mShaderResourceView;
}
