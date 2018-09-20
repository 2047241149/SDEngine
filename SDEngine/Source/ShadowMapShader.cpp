#include "ShadowMapShader.h"

ShadowMapShader::ShadowMapShader(WCHAR* vsFilenPath, WCHAR* psFilenPath):
	Shader_2D(vsFilenPath,psFilenPath),
	mCBCommon(nullptr),
	mCBShadowMap(nullptr)
{
	CreateConstantBuffer();
	CreateSampler();
}


ShadowMapShader::ShadowMapShader(const ShadowMapShader& other):
	Shader_2D(other)
{

}


ShadowMapShader::~ShadowMapShader()
{
	ShutDown();
}



bool ShadowMapShader::SetShaderParams(ID3D11ShaderResourceView* worldPosTex, ID3D11ShaderResourceView*  lightDepthMap, int nDirLightIndex)
{
	bool result;
	//设置Shader常量缓存和纹理资源
	result = SetShaderCB(worldPosTex, lightDepthMap, nDirLightIndex);
	if (!result)
		return false;

	//设置VertexShader PixelShader InputLayout SamplerState
	SetShaderState();

	return true;
}




void ShadowMapShader::CreateConstantBuffer()
{

	D3D11_BUFFER_DESC commonBufferDesc;
	ZeroMemory(&commonBufferDesc, sizeof(commonBufferDesc));
	commonBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	commonBufferDesc.ByteWidth = sizeof(CBCommmon);   
	commonBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	commonBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	g_pDevice->CreateBuffer(&commonBufferDesc, NULL, &mCBCommon);

	D3D11_BUFFER_DESC shadowMapBufferDesc;
	ZeroMemory(&shadowMapBufferDesc, sizeof(shadowMapBufferDesc));
	shadowMapBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	shadowMapBufferDesc.ByteWidth = sizeof(CBShadowMap);
	shadowMapBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	shadowMapBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	g_pDevice->CreateBuffer(&shadowMapBufferDesc, NULL, &mCBShadowMap);
}


void ShadowMapShader::CreateSampler()
{
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	g_pDevice->CreateSamplerState(&samplerDesc, &mClampPointSampler);
}


void ShadowMapShader::ShutDown()
{
	ReleaseCOM(mCBCommon);
	ReleaseCOM(mCBShadowMap);
	ReleaseCOM(mClampPointSampler);
}


bool ShadowMapShader::SetShaderCB(ID3D11ShaderResourceView* worldPosTex, ID3D11ShaderResourceView*  lightDepthMap, int nDirLightIndex)
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
	pCBCommon->cameraPos = GCamera->GetPosition();
	g_pDeviceContext->Unmap(mCBCommon, 0);

	shared_ptr<DirectionLight> pDirLight = GLightManager->m_vecDirLight[nDirLightIndex];
	D3D11_MAPPED_SUBRESOURCE mappedSSShadowMap;
	HR(g_pDeviceContext->Map(mCBShadowMap, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSSShadowMap));
	auto pCBShadowMap = reinterpret_cast<CBShadowMap*>(mappedSSShadowMap.pData);

	XMMATRIX lightViewMatrix;
	XMMATRIX lightOrthoProjMatrix;
	pDirLight->GetDirLightViewAndProjMatrix(lightViewMatrix, lightOrthoProjMatrix);
	XMFLOAT3 lightColor = pDirLight->GetLightColor();
	pCBShadowMap->lightViewMatrix = XMMatrixTranspose(lightViewMatrix);
	pCBShadowMap->lightProjMatrix = XMMatrixTranspose(lightOrthoProjMatrix);
	g_pDeviceContext->Unmap(mCBShadowMap, 0);

	//第三,设置在VertexShader的常量缓存的值(带着更新的值)
	g_pDeviceContext->VSSetConstantBuffers(0, 1, &mCBCommon);
	g_pDeviceContext->PSSetConstantBuffers(0, 1, &mCBCommon);
	g_pDeviceContext->PSSetConstantBuffers(1, 1, &mCBShadowMap);
	g_pDeviceContext->PSSetShaderResources(0, 1, &worldPosTex);
	g_pDeviceContext->PSSetShaderResources(1, 1, &lightDepthMap);

	return true;
}

void ShadowMapShader::SetShaderState()
{
	Shader_2D::SetShaderState();
	g_pDeviceContext->PSSetSamplers(2, 1, &mClampPointSampler);
}

