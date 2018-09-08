#include "FxaaShader.h"


FxaaShader::FxaaShader(WCHAR* vsFilenPath, WCHAR* psFilenPath):
	Shader_2D(vsFilenPath,psFilenPath),
	mCBFxaa(nullptr)
{
	CreateConstantBuffer();
	CreateSamplerState();
}


FxaaShader::FxaaShader(const FxaaShader& other):
	Shader_2D(other)
{

}


FxaaShader::~FxaaShader()
{
	ShutDown();
}


bool FxaaShader::SetShaderParams(ID3D11ShaderResourceView* screenRT, float fScrreenWidth, float fScrreenHeight)
{
	bool result;
	//设置Shader常量缓存和纹理资源
	result = SetShaderCB(screenRT, fScrreenWidth, fScrreenHeight);
	if (!result)
		return false;

	//设置VertexShader PixelShader InputLayout SamplerState
	SetShaderState();

	return true;
}


void FxaaShader::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC cBufferDesc;
	ZeroMemory(&cBufferDesc, sizeof(cBufferDesc));
	cBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cBufferDesc.ByteWidth = sizeof(CBFXAA); 
	cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	g_pDevice->CreateBuffer(&cBufferDesc, NULL, &mCBFxaa);
}




void FxaaShader::ShutDown()
{
	ReleaseCOM(mCBFxaa);
}


bool FxaaShader::SetShaderCB(ID3D11ShaderResourceView* screenRT, float fScrreenWidth, float fScrreenHeight)
{

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	HR(g_pDeviceContext->Map(mCBFxaa, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource));
	auto pCBFxaa = reinterpret_cast<CBFXAA*>(mappedSubresource.pData);
	/*IDXGISurface* pDXGISurface = nullptr;
	DXGI_SURFACE_DESC* pDXGISurfaceDesc = nullptr;
	g_DXGIOutput->GetDisplaySurfaceData(pDXGISurface);
	pDXGISurface->GetDesc(pDXGISurfaceDesc);*/
	pCBFxaa->rcpFrame.x = 1.0 / fScrreenWidth;
	pCBFxaa->rcpFrame.y = 1.0 / fScrreenHeight;
	pCBFxaa->rcpFrame.z = 0.0;
	pCBFxaa->rcpFrame.w = 0.0;
	g_pDeviceContext->Unmap(mCBFxaa, 0);

	g_pDeviceContext->PSSetConstantBuffers(0, 1, &mCBFxaa);
	g_pDeviceContext->PSSetShaderResources(0, 1, &screenRT);

	return true;
}

void FxaaShader::SetShaderState()
{
	Shader_2D::SetShaderState();
	g_pDeviceContext->PSSetSamplers(2, 0, &mAnisotropicSampler);

}

void FxaaShader::CreateSamplerState()
{
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 4;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	g_pDevice->CreateSamplerState(&samplerDesc, &mAnisotropicSampler);
}


