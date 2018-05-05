#include "ScreenSpaceReflectShader.h"

SSRShader::SSRShader(WCHAR* vsFilenPath, WCHAR* psFilenPath):Shader(vsFilenPath, psFilenPath)
{
	CreateBuffer();
}


SSRShader::SSRShader(const SSRShader& other) :Shader(other)
{

}


SSRShader::~SSRShader()
{
	ShutDown();
}

void SSRShader::ShutDown()
{

}

void SSRShader::CreateBuffer()
{
	ID3D11Device* d3dDevice = D3DClass::GetInstance()->GetDevice();
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(CBSSR);   //结构体大小,必须为16字节倍数
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	d3dDevice->CreateBuffer(&bufferDesc, NULL, &mCBSSR);
}

bool SSRShader::SetShaderCBExtern(CXMMATRIX worldMatrix, ID3D11ShaderResourceView* diffuseTex,
	ID3D11ShaderResourceView* depthTex,
	float viewAngleThresshold, float edgeDistThresshold,
	float depthBias, float reflectScale, XMFLOAT4 perspectiveValues)
{
	ID3D11DeviceContext* d3dDeviceContext = D3DClass::GetInstance()->GetDeviceContext();
	Shader::SetShaderCB(worldMatrix);

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	HR(d3dDeviceContext->Map(mCBSSR, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource));
	auto pCBSSR = reinterpret_cast<CBSSR*>(mappedSubresource.pData);
	pCBSSR->depthBias = depthBias;
	pCBSSR->edgeDistThresshold = edgeDistThresshold;
	pCBSSR->reflectScale = reflectScale;
	pCBSSR->pectiveValues = perspectiveValues;
	pCBSSR->viewAngleThresshold = viewAngleThresshold;
	d3dDeviceContext->Unmap(mCBSSR, 0);

	d3dDeviceContext->PSSetConstantBuffers(1, 1, &mCBSSR);
	d3dDeviceContext->PSSetShaderResources(0, 1, &diffuseTex);
	d3dDeviceContext->PSSetShaderResources(1, 1, &depthTex);


	return true;
}


bool SSRShader::SetShaderParamsExtern(CXMMATRIX worldMatrix, ID3D11ShaderResourceView* diffuseTex,
	ID3D11ShaderResourceView* depthTex,
	float viewAngleThresshold, float edgeDistThresshold,
	float depthBias, float reflectScale, XMFLOAT4 perspectiveValues)
{

	bool result;

	//设置Shader常量缓存和纹理资源
	result = SetShaderCBExtern(worldMatrix, diffuseTex,depthTex, 
		viewAngleThresshold, edgeDistThresshold,
		depthBias, reflectScale,perspectiveValues);
	if (!result)
		return false;

	//设置VertexShader PixelShader InputLayout SamplerState
	SetShaderState();

	return true;
}