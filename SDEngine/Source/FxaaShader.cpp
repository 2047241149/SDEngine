#include "FxaaShader.h"


FxaaShader::FxaaShader(WCHAR* vsFilenPath, WCHAR* psFilenPath):
	Shader_2D(vsFilenPath,psFilenPath),
	mCBFxaa(nullptr)
{

}


FxaaShader::FxaaShader(const FxaaShader& other):
	Shader_2D(other)
{

}


FxaaShader::~FxaaShader()
{
	ShutDown();
}


bool FxaaShader::SetShaderParams(ID3D11ShaderResourceView* screenRT, ID3D11ShaderResourceView* screenBlurRT,
	ID3D11ShaderResourceView* depthRT,
	float dofStart, float dofRange,float farPlane, float nearPlane)
{
	bool result;
	//设置Shader常量缓存和纹理资源
	result = SetShaderCB(screenRT,screenBlurRT, depthRT,dofStart,dofRange,farPlane,nearPlane);
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


bool FxaaShader::SetShaderCB(ID3D11ShaderResourceView* screenRT, ID3D11ShaderResourceView* screenBlurRT,
	ID3D11ShaderResourceView* depthRT,
	float dofStart, float dofRange, float farPlane, float nearPlane)
{

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	HR(g_pDeviceContext->Map(mCBFxaa, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource));
	auto pCBFxaa = reinterpret_cast<CBFXAA*>(mappedSubresource.pData);
	g_pDeviceContext->Unmap(mCBFxaa, 0);

	g_pDeviceContext->PSSetConstantBuffers(0, 1, &mCBFxaa);
	g_pDeviceContext->PSSetShaderResources(0, 1, &screenRT);
	g_pDeviceContext->PSSetShaderResources(1, 1, &screenBlurRT);
	g_pDeviceContext->PSSetShaderResources(2, 1, &depthRT);

	return true;
}
