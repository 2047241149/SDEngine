#include "WaveShader.h"

WaveShader::WaveShader(WCHAR* vsFilenPath, WCHAR* psFilenPath) :Shader(vsFilenPath, psFilenPath)
{
	CreateBuffer();
}


WaveShader::WaveShader(const WaveShader& other) :Shader(other)
{

}


WaveShader::~WaveShader()
{
	ShutDown();
}


void WaveShader::CreateBuffer()
{

	ID3D11Device* d3dDevice = D3DClass::GetInstance()->GetDevice();
	//CBEveryFrame
	D3D11_BUFFER_DESC cbufferDesc;
	ZeroMemory(&cbufferDesc, sizeof(cbufferDesc));
	cbufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbufferDesc.ByteWidth = sizeof(CBEveryFrame);   //结构体大小,必须为16字节倍数
	cbufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	d3dDevice->CreateBuffer(&cbufferDesc, NULL, &mCBEveryFrameBuffer);
}



void WaveShader::ShutDown()
{
	ReleaseCOM(mCBEveryFrameBuffer);
}

bool WaveShader::SetShaderCBExtern(CXMMATRIX worldMatrix, FXMVECTOR surfaceColor, ID3D11ShaderResourceView* diffuse, ID3D11ShaderResourceView* normal)
{
	ID3D11DeviceContext* d3dDeviceContext = D3DClass::GetInstance()->GetDeviceContext();
	Shader::SetShaderCB(worldMatrix);

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	HR(d3dDeviceContext->Map(mCBEveryFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource));
	auto pCBEverFrame = reinterpret_cast<CBEveryFrame*>(mappedSubresource.pData);
	XMStoreFloat4(&pCBEverFrame->surfaceColor, surfaceColor);
	d3dDeviceContext->Unmap(mCBEveryFrameBuffer, 0);
	d3dDeviceContext->PSSetConstantBuffers(1, 1, &mCBEveryFrameBuffer);
	d3dDeviceContext->PSSetShaderResources(0, 1, &diffuse);
	d3dDeviceContext->PSSetShaderResources(1, 1, &normal);

	return true;
}
bool WaveShader::SetShaderParamsExtern(CXMMATRIX worldMatrix, FXMVECTOR surfaceColor,
	ID3D11ShaderResourceView* diffuse, ID3D11ShaderResourceView* normal)
{
	bool result;

	//设置Shader常量缓存和纹理资源
	result = SetShaderCBExtern(worldMatrix, surfaceColor, diffuse,normal);
	if (!result)
		return false;

	//设置VertexShader PixelShader InputLayout SamplerState
	SetShaderState();
	return true;
}