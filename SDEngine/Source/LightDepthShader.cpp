#include "LightDepthShader.h"

LightDepthShader::LightDepthShader(WCHAR* vsFilenPath, WCHAR* psFilenPath):
	Shader_3D(vsFilenPath, psFilenPath)
{

}


LightDepthShader::LightDepthShader(const LightDepthShader& other):
	Shader_3D(other)
{

}


LightDepthShader::~LightDepthShader()
{
	
}

bool LightDepthShader::SetShaderCB(CXMMATRIX worldMatrix)
{
	XMMATRIX lightViewMatrix;
	XMMATRIX lightOrthoProjMatrix;
	GLightManager->GetMainLight()->GetDirLightViewAndProjMatrix(lightViewMatrix, lightOrthoProjMatrix);

	//第一，更新变换矩阵常量缓存的值
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	HR(g_pDeviceContext->Map(m_pCBCommon, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource));
	auto pCBCommon = reinterpret_cast<CBCommmon*>(mappedSubresource.pData);
	XMMATRIX worldMa = XMMatrixTranspose(worldMatrix);
	XMMATRIX viewMa = XMMatrixTranspose(lightViewMatrix);
	XMMATRIX ProjMa = XMMatrixTranspose(lightOrthoProjMatrix);
	XMMATRIX worldInvTranspose = XMMatrixTranspose(MathTool::GetInvenseTranspose(worldMatrix));
	pCBCommon->mWorldMatrix = worldMa;
	pCBCommon->mViewMatrix = viewMa;
	pCBCommon->mProjMatrix = ProjMa;
	pCBCommon->mWorldInvTranposeMatirx = worldInvTranspose;
	pCBCommon->cameraPos = GCamera->GetPosition();
	g_pDeviceContext->Unmap(m_pCBCommon, 0);

	//第三,设置在VertexShader的常量缓存的值(带着更新的值)
	g_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pCBCommon);
	g_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pCBCommon);

	return true;
}
