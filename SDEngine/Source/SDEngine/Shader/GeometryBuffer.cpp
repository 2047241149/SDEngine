#include "GeometryBuffer.h"

GeometryBuffer::GeometryBuffer(int TextureWidth, int TexureHeight, float ScreenDepth, float ScreenNear)
{
	for (int i = 0; i < BUFFER_COUNT; ++i)
	{
		mRenderTargetTextureArray[i] = NULL;
		mRenderTargetViewArray[i] = NULL;
		mGBufferSRV[i] = NULL;
	}
	mDepthStencilTexture = NULL;
	mDepthStencilView = NULL;
	mDepthBufferSRV = NULL;

	mRandomTexture = NULL;
	mRandomSRV = NULL;

	Initialize(TextureWidth, TexureHeight, ScreenDepth, ScreenNear);
}


GeometryBuffer::GeometryBuffer(const GeometryBuffer&other)
{

}

GeometryBuffer::~GeometryBuffer()
{

}


bool GeometryBuffer::Initialize(int TextureWidth, int TextureHeight, float ScreenDepth, float ScreenNear)
{

	//*************************创建GBuffer和RenderTargetView****************************************//

	//创建GBuffer的texture
	D3D11_TEXTURE2D_DESC gBufferTextureDesc;
	ZeroMemory(&gBufferTextureDesc, sizeof(gBufferTextureDesc));

	gBufferTextureDesc.Width = TextureWidth;
	gBufferTextureDesc.Height = TextureHeight;
	gBufferTextureDesc.MipLevels = 1;
	gBufferTextureDesc.ArraySize = 1;
	gBufferTextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;  //纹理像素为12个字节
	gBufferTextureDesc.SampleDesc.Count = 1;
	gBufferTextureDesc.SampleDesc.Quality = 0;
	gBufferTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	gBufferTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	gBufferTextureDesc.CPUAccessFlags = 0;
	gBufferTextureDesc.MiscFlags = 0;



	for (int i = 0; i < BUFFER_COUNT; ++i)
	{
		HR(g_pDevice->CreateTexture2D(&gBufferTextureDesc, NULL, &mRenderTargetTextureArray[i]));
	}


	//创建GBuffer对应的RenderTargetView
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;

	renderTargetViewDesc.Format = gBufferTextureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	for (int i = 0; i < BUFFER_COUNT; ++i)
	{
		HR(g_pDevice->CreateRenderTargetView(mRenderTargetTextureArray[i], &renderTargetViewDesc, &mRenderTargetViewArray[i]));
	}



	//利用RenderTargetView创建相应的GBuffer ShadreView
	D3D11_SHADER_RESOURCE_VIEW_DESC gBufferShaderResourceViewDesc;
	gBufferShaderResourceViewDesc.Format = gBufferTextureDesc.Format;
	gBufferShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	gBufferShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	gBufferShaderResourceViewDesc.Texture2D.MipLevels = 1;

	for (int i = 0; i < BUFFER_COUNT; ++i)
	{
		HR(g_pDevice->CreateShaderResourceView(mRenderTargetTextureArray[i], &gBufferShaderResourceViewDesc, 
			&mGBufferSRV[i]));
	}

	//*************************创建DepthBuffer和DepthStencilView*************************************************//

	//创建DepthBuffer的Desc
	D3D11_TEXTURE2D_DESC depthBufferTextureDesc;
	ZeroMemory(&depthBufferTextureDesc, sizeof(depthBufferTextureDesc));
	depthBufferTextureDesc.Width = TextureWidth;
	depthBufferTextureDesc.Height = TextureHeight;
	depthBufferTextureDesc.MipLevels = 1;
	depthBufferTextureDesc.ArraySize = 1;
	depthBufferTextureDesc.Format = DXGI_FORMAT_R24G8_TYPELESS; //24位是为了深度缓存，8位是为了模板缓存  
	depthBufferTextureDesc.SampleDesc.Count = 1;
	depthBufferTextureDesc.SampleDesc.Quality = 0;
	depthBufferTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;  //注意深度缓存(纹理)的绑定标志  
	depthBufferTextureDesc.CPUAccessFlags = 0;
	depthBufferTextureDesc.MiscFlags = 0;
	HR(g_pDevice->CreateTexture2D(&depthBufferTextureDesc, NULL, &mDepthStencilTexture));


	//第五,填充深度缓存视图形容结构体,创建深度缓存(模板缓存)视图
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	HR(g_pDevice->CreateDepthStencilView(
		mDepthStencilTexture, //我们基于这个深度缓存/漏字板缓存创建一个视图
		&depthStencilViewDesc,
		&mDepthStencilView));//指向深度缓存/漏字板视图的指针


	D3D11_SHADER_RESOURCE_VIEW_DESC depthBufferSRVDesc;
	depthBufferSRVDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	depthBufferSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	depthBufferSRVDesc.Texture2D.MostDetailedMip = 0;
	depthBufferSRVDesc.Texture2D.MipLevels = depthBufferTextureDesc.MipLevels;

	HR(g_pDevice->CreateShaderResourceView(mDepthStencilTexture, &depthBufferSRVDesc,
		&mDepthBufferSRV));


	//创建randomRT
	srand(static_cast<unsigned int>(time(NULL)));
	unsigned char* rand_data = new unsigned char[TextureWidth * TextureHeight];
	for (int i = 0; i < TextureHeight; ++i)
	{
		for (int j = 0; j < TextureWidth; ++j)
		{
			//取值的前八位,也就0到255
			unsigned char value = static_cast<unsigned char>(rand())&static_cast<unsigned char>(0x00ff);
			rand_data[i*TextureWidth + j] = value;
		}
	}

	D3D11_TEXTURE2D_DESC randomRTDesc;
	randomRTDesc.Width = TextureWidth;
	randomRTDesc.Height = TextureHeight;
	randomRTDesc.MipLevels = 1;
	randomRTDesc.ArraySize = 1;
	randomRTDesc.Format = DXGI_FORMAT_R8_UNORM;  //纹理像素为12个字节
	randomRTDesc.SampleDesc.Count = 1;
	randomRTDesc.SampleDesc.Quality = 0;
	randomRTDesc.Usage = D3D11_USAGE_DEFAULT;
	randomRTDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	randomRTDesc.CPUAccessFlags = 0;
	randomRTDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA tex_data;

	tex_data.pSysMem = rand_data;
	tex_data.SysMemPitch = TextureWidth;
	tex_data.SysMemSlicePitch = TextureWidth * TextureHeight;
	g_pDevice->CreateTexture2D(&randomRTDesc, &tex_data, &mRandomTexture);
	g_pDevice->CreateShaderResourceView(mRandomTexture, NULL, &mRandomSRV);

	delete[] rand_data;

	/*对应于GBuffer和DepthBuffer的视口大小*/
	//设置渲染的视口
	md3dViewport.Width = static_cast<float>(TextureWidth);
	md3dViewport.Height = static_cast<float>(TextureHeight);
	md3dViewport.MinDepth = 0.0f;
	md3dViewport.MaxDepth = 1.0f;
	md3dViewport.TopLeftX = 0.0f;
	md3dViewport.TopLeftY = 0.0f;

	return true;

}


void GeometryBuffer::ShutDown()
{
	for (int i = 0; i < BUFFER_COUNT; ++i)
	{
		ReleaseCOM(mGBufferSRV[i]);
		ReleaseCOM(mRenderTargetTextureArray[i]);
		ReleaseCOM(mRenderTargetViewArray[i]);
	}
	ReleaseCOM(mDepthStencilTexture);
	ReleaseCOM(mDepthStencilView);
	ReleaseCOM(mDepthBufferSRV);

	ReleaseCOM(mRandomSRV);
	ReleaseCOM(mRandomTexture);
}


//让此时所有图形渲染到这个目前渲染的位置
void GeometryBuffer::SetRenderTarget(XMFLOAT3 backColor)
{
	//绑定渲染目标视图和深度模板视图到输出渲染管线，此时渲染输出到两张纹理中
	g_pDeviceContext->OMSetRenderTargets(BUFFER_COUNT, mRenderTargetViewArray, mDepthStencilView);

	//设置相应的视口
	g_pDeviceContext->RSSetViewports(1, &md3dViewport);

	ClearGBuffer(backColor);

}

void GeometryBuffer::SetDepthTarget()
{
	ID3D11RenderTargetView* nullRT[1] = { nullptr };
	g_pDeviceContext->OMSetRenderTargets(1, nullRT, mDepthStencilView);
	g_pDeviceContext->RSSetViewports(1, &md3dViewport);
	ClearDepthBuffer();
}

void GeometryBuffer::ClearDepthBuffer()
{

	//清除深度缓存和模板缓存
	g_pDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
}



void GeometryBuffer::ClearGBuffer(XMFLOAT3 backColor)
{
	//设置清除缓存为的颜色
	float color[4];
	color[0] = backColor.x;
	color[1] = backColor.y;
	color[2] = backColor.z;
	color[3] = 1.0f;

	//ColorBuffer
	g_pDeviceContext->ClearRenderTargetView(mRenderTargetViewArray[GBufferType::Diffuse], color);
	
	
	//WorldNormal
	color[0] = 0.0f;
	color[1] = 0.0f;
	color[2] = -1.0f;
	g_pDeviceContext->ClearRenderTargetView(mRenderTargetViewArray[GBufferType::Normal], color);

	//WorldPos
	color[0] = 0.0f;
	color[1] = 0.0f;
	color[2] = 0.0f;
	g_pDeviceContext->ClearRenderTargetView(mRenderTargetViewArray[GBufferType::Pos], color);

	//Specular-Rough-Metal
	color[0] = 1.0f;
	color[1] = 1.0f;
	color[2] = 1.0f;
	g_pDeviceContext->ClearRenderTargetView(mRenderTargetViewArray[GBufferType::SpecularRoughMetal], color);

}

void GeometryBuffer::ClearRenderTarget(XMFLOAT3 backColor)
{
	ClearDepthBuffer();
	ClearGBuffer(backColor);
}

//
ID3D11ShaderResourceView* GeometryBuffer::GetGBufferSRV(GBufferType gBufferType)
{
	if (gBufferType == GBufferType::Depth)
	{
		return mDepthBufferSRV;
	}
	else
	{
		return mGBufferSRV[gBufferType];
	}
	
}

ID3D11ShaderResourceView* GeometryBuffer::GetRandomRTSRV()
{
	return mRandomSRV;
}

ID3D11DepthStencilView* GeometryBuffer::GetDSV()
{
	return mDepthStencilView;
}