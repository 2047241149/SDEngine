#include"DefferLightingShader.h"

DefferLighingShader::DefferLighingShader(WCHAR* vsFilenPath, WCHAR* psFilenPath):
	Shader_2D(vsFilenPath,psFilenPath),
	mCBCommon(nullptr)
{
	CreateConstantBuffer();
}


DefferLighingShader::DefferLighingShader(const DefferLighingShader& other):
	Shader_2D(other)
{

}


DefferLighingShader::~DefferLighingShader()
{
	ShutDown();
}




bool DefferLighingShader::SetShaderParams(ID3D11ShaderResourceView* gBuffer[4])
{
	bool result;
	//设置Shader常量缓存和纹理资源
	result = SetShaderCB(gBuffer);
	if (!result)
		return false;

	//设置VertexShader PixelShader InputLayout SamplerState
	SetShaderState();

	return true;
}




void DefferLighingShader::CreateConstantBuffer()
{

	D3D11_BUFFER_DESC commonBufferDesc;
	ZeroMemory(&commonBufferDesc, sizeof(commonBufferDesc));
	commonBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	commonBufferDesc.ByteWidth = sizeof(CBCommmon);   
	commonBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	commonBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	g_pDevice->CreateBuffer(&commonBufferDesc, NULL, &mCBCommon);
}



void DefferLighingShader::ShutDown()
{
	ReleaseCOM(mCBCommon);
}


bool DefferLighingShader::SetShaderCB(ID3D11ShaderResourceView* gBuffer[4])
{
	XMMATRIX viewMatrix = GCamera->GetViewMatrix();
	XMMATRIX ProjMatrix = GCamera->GetProjectionMatrix();

	//第一，更新变换矩阵常量缓存的值
	//将矩阵转置,在传入常量缓存前进行转置,因为GPU对矩阵数据会自动进行一次转置
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	HR(g_pDeviceContext->Map(mCBCommon, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource));
	auto pCBCommon = reinterpret_cast<CBCommmon*>(mappedSubresource.pData);
	XMMATRIX worldMa = XMMatrixIdentity();
	XMMATRIX viewMa = XMMatrixTranspose(viewMatrix);
	XMMATRIX ProjMa = XMMatrixTranspose(ProjMatrix);
	pCBCommon->mWorldMatrix = worldMa;
	pCBCommon->mViewMatrix = viewMa;
	pCBCommon->mProjMatrix = ProjMa;
	pCBCommon->mWorldInvTranposeMatirx = XMMatrixIdentity();
	pCBCommon->dirLightColor = GLightManager->GetMainLight()->GetLightColor();
	pCBCommon->dirLightDir = GLightManager->GetMainLight()->GetLightDirection();
	pCBCommon->ambientLight = GLightManager->GetMainLight()->GetAmbientLight();
	pCBCommon->cameraPos = GCamera->GetPosition();
	g_pDeviceContext->Unmap(mCBCommon, 0);
	//第三,设置在VertexShader的常量缓存的值(带着更新的值)
	g_pDeviceContext->VSSetConstantBuffers(0, 1, &mCBCommon);
	g_pDeviceContext->PSSetConstantBuffers(0, 1, &mCBCommon);
	g_pDeviceContext->PSSetShaderResources(0, 4, gBuffer);

	return true;
}



