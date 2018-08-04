#include "DOFShader.h"


DOFShader::DOFShader(WCHAR* vsFilenPath, WCHAR* psFilenPath):
	Shader_2D(vsFilenPath,psFilenPath),
	mCBDOF(nullptr)
{

}


DOFShader::DOFShader(const DOFShader& other):
	Shader_2D(other)
{

}


DOFShader::~DOFShader()
{
	ShutDown();
}


bool DOFShader::SetShaderParams(ID3D11ShaderResourceView* screenRT, ID3D11ShaderResourceView* screenBlurRT,
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


void DOFShader::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC cBufferDesc;
	ZeroMemory(&cBufferDesc, sizeof(cBufferDesc));
	cBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cBufferDesc.ByteWidth = sizeof(CBDOF);   //结构体大小,必须为16字节倍数
	cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	g_pDevice->CreateBuffer(&cBufferDesc, NULL, &mCBDOF);
}




void DOFShader::ShutDown()
{
	ReleaseCOM(mCBDOF);
}


bool DOFShader::SetShaderCB(ID3D11ShaderResourceView* screenRT, ID3D11ShaderResourceView* screenBlurRT,
	ID3D11ShaderResourceView* depthRT,
	float dofStart, float dofRange, float farPlane, float nearPlane)
{

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	HR(g_pDeviceContext->Map(mCBDOF, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource));
	auto pCBDOF = reinterpret_cast<CBDOF*>(mappedSubresource.pData);
	pCBDOF->depthRange = dofRange;
	pCBDOF->depthStart = dofStart;
	pCBDOF->farPlane = farPlane;
	pCBDOF->nearPlane = nearPlane;
	g_pDeviceContext->Unmap(mCBDOF, 0);

	g_pDeviceContext->PSSetConstantBuffers(0, 1, &mCBDOF);
	g_pDeviceContext->PSSetShaderResources(0, 1, &screenRT);
	g_pDeviceContext->PSSetShaderResources(1, 1, &screenBlurRT);
	g_pDeviceContext->PSSetShaderResources(2, 1, &depthRT);

	return true;
}
