#include"PureColorShader.h"

PureColorShader::PureColorShader(WCHAR* vsFilenPath, WCHAR* psFilenPath) :Shader(vsFilenPath, psFilenPath)
{
	CreateBuffer();
}


PureColorShader::PureColorShader(const PureColorShader& other) :Shader(other)
{

}


PureColorShader::~PureColorShader()
{
	ShutDown();
}


void PureColorShader::CreateBuffer()
{

	//CBEveryFrame
	D3D11_BUFFER_DESC cbufferDesc;
	ZeroMemory(&cbufferDesc, sizeof(cbufferDesc));
	cbufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbufferDesc.ByteWidth = sizeof(CBEveryFrame);   //结构体大小,必须为16字节倍数
	cbufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	g_pDevice->CreateBuffer(&cbufferDesc, NULL, &mCBEveryFrameBuffer);
}



void PureColorShader::ShutDown()
{
	ReleaseCOM(mCBEveryFrameBuffer);
}

bool PureColorShader::SetShaderCBExtern(CXMMATRIX worldMatrix, FXMVECTOR surfaceColor)
{

	Shader::SetShaderCB(worldMatrix);

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	HR(g_pDeviceContext->Map(mCBEveryFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource));
	auto pCBEverFrame = reinterpret_cast<CBEveryFrame*>(mappedSubresource.pData);
	XMStoreFloat4(&pCBEverFrame->surfaceColor, surfaceColor);
	g_pDeviceContext->Unmap(mCBEveryFrameBuffer, 0);
	g_pDeviceContext->PSSetConstantBuffers(1, 1, &mCBEveryFrameBuffer);

	return true;
}
bool PureColorShader::SetShaderParamsExtern(CXMMATRIX worldMatrix, FXMVECTOR surfaceColor)
{
	bool result;

	//设置Shader常量缓存和纹理资源
	result = SetShaderCBExtern(worldMatrix, surfaceColor);
	if (!result)
		return false;

	//设置VertexShader PixelShader InputLayout SamplerState
	SetShaderState();
	return true;
}