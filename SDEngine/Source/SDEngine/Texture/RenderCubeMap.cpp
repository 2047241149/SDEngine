#include "RenderCubeMap.h"

RenderCubeMap::RenderCubeMap(int inTextureWidth, int inTextureHeight):
	textureWidth(inTextureWidth),
	textureHeight(inTextureHeight),
	dsv(nullptr),
	depthTexture(nullptr),
	srv(nullptr),
	cubeMapTexture(nullptr)
{
	for (int index = 0; index < 6; ++index)
	{
		rtvs[index] = nullptr;
	}

	Init(inTextureWidth, inTextureHeight);
}

RenderCubeMap::~RenderCubeMap()
{
	ShutDown();
}

bool RenderCubeMap::Init(int nTextureWidth, int nTexureHeight)
{
	//第一,填充2D纹理形容结构体,并创建2D渲染目标纹理
	D3D11_TEXTURE2D_DESC cubeMapTextureDesc;
	ZeroMemory(&cubeMapTextureDesc, sizeof(cubeMapTextureDesc));
	cubeMapTextureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	cubeMapTextureDesc.Width = nTextureWidth;
	cubeMapTextureDesc.Height = nTexureHeight;
	cubeMapTextureDesc.MipLevels = 0;
	cubeMapTextureDesc.ArraySize = 6;
	cubeMapTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	cubeMapTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	cubeMapTextureDesc.CPUAccessFlags = 0;
	cubeMapTextureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_GENERATE_MIPS;
	cubeMapTextureDesc.SampleDesc.Count = 1;
	cubeMapTextureDesc.SampleDesc.Quality = 0;
	HR(g_pDevice->CreateTexture2D(&cubeMapTextureDesc, NULL, &cubeMapTexture));

	//第二，填充渲染目标视图形容体,并进行创建目标渲染视图
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = cubeMapTextureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	renderTargetViewDesc.Texture2DArray.ArraySize = 1;

	for (int index = 0; index < 6; ++index)
	{
		renderTargetViewDesc.Texture2DArray.FirstArraySlice = index;
		HR(g_pDevice->CreateRenderTargetView(cubeMapTexture, &renderTargetViewDesc, &rtvs[index]));
	}
	
	//第三,创建着色器资源视图
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = cubeMapTextureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = -1;
	HR(g_pDevice->CreateShaderResourceView(cubeMapTexture, &shaderResourceViewDesc, &srv));
	

	//第四,创建深度缓存(模板缓存)
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.Width = nTextureWidth;
	depthStencilDesc.Height = nTexureHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	HR(g_pDevice->CreateTexture2D(&depthStencilDesc,//要创建的纹理的形容
		0,
		&depthTexture));

	//第五,创建深度缓存(模板缓存)视图
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = depthStencilDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	HR(g_pDevice->CreateDepthStencilView(
		depthTexture, //我们基于这个深度缓存/漏字板缓存创建一个视图
		&depthStencilViewDesc,
		&dsv));//指向深度缓存/漏字板视图的指针

	//第六,设置渲染的视口
	viewport.Width = static_cast<float>(nTextureWidth);
	viewport.Height = static_cast<float>(nTexureHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	return true;
 }

void RenderCubeMap::ShutDown()
{
	ReleaseCOM(srv);
	ReleaseCOM(cubeMapTexture);

	for (int index = 0; index < 6; ++index)
	{
		ReleaseCOM(rtvs[index]);
	}

	ReleaseCOM(depthTexture);
	ReleaseCOM(dsv);
}

//让此时所有图形渲染到这个目前渲染的位置
void RenderCubeMap::SetRenderTarget(int targetViewSlot)
{
	//绑定渲染目标视图和深度模板视图到输出渲染管线，此时渲染输出到两张纹理中
	g_pDeviceContext->OMSetRenderTargets(1, &rtvs[targetViewSlot], dsv);

	//设置相应的视口
	g_pDeviceContext->RSSetViewports(1, &viewport);
}


void RenderCubeMap::ClearDepthBuffer()
{
	//清除深度缓存和模板缓存
	g_pDeviceContext->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void RenderCubeMap::ClearRenderTarget(float red, float green, float blue, float alpha)
{
	//设置清除缓存为的颜色
	float color[4];
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	for (int index = 0; index < 6; ++index)
	{
		g_pDeviceContext->ClearRenderTargetView(rtvs[index], color);
	}
}

ID3D11ShaderResourceView* RenderCubeMap::GetSRV()
{
	return srv;
}

ID3D11RenderTargetView* RenderCubeMap::GetRenderTargetView(int targetViewSlot)
{
	return rtvs[targetViewSlot];
}