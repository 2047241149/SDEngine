#include "ShadowMapShader.h"
#include "CascadeShadowMapManager.h"

ShadowMapShader::ShadowMapShader(WCHAR* vsFilenPath, WCHAR* psFilenPath):
	Shader_2D(vsFilenPath,psFilenPath),
	mCBCommon(nullptr),
	mCBShadowMap(nullptr),
	mBorderLinearSample(nullptr)
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



bool ShadowMapShader::SetShaderParams(ID3D11ShaderResourceView* worldPosTex, CascadedShadowsManager* mCascadeShadowManager)
{
	bool result;
	//����Shader���������������Դ
	result = SetShaderCB(worldPosTex, mCascadeShadowManager);
	if (!result)
		return false;

	//����VertexShader PixelShader InputLayout SamplerState
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
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 0.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 0.0f;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	g_pDevice->CreateSamplerState(&samplerDesc, &mBorderLinearSample);
}


void ShadowMapShader::ShutDown()
{
	ReleaseCOM(mCBCommon);
	ReleaseCOM(mCBShadowMap);
	ReleaseCOM(mBorderLinearSample);
}


bool ShadowMapShader::SetShaderCB(ID3D11ShaderResourceView* worldPosTex, CascadedShadowsManager* mCascadeShadowManager)
{

	if (GLightManager->m_vecDirLight.size() <= 0)
		return false;

	XMMATRIX viewMatrix = GCamera->GetViewMatrix();
	XMMATRIX ProjMatrix = GCamera->GetProjectionMatrix();

	//��һ�����±任�����������ֵ
	//������ת��,�ڴ��볣������ǰ����ת��,��ΪGPU�Ծ������ݻ��Զ�����һ��ת��
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

	shared_ptr<DirectionLight> pDirLight = GLightManager->m_vecDirLight[0];
	D3D11_MAPPED_SUBRESOURCE mappedSSShadowMap;
	HR(g_pDeviceContext->Map(mCBShadowMap, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSSShadowMap));
	auto pCBShadowMap = reinterpret_cast<CBShadowMap*>(mappedSSShadowMap.pData);

	XMMATRIX lightViewMatrix;
	lightViewMatrix = pDirLight->GetViewMatrix();
	XMFLOAT3 lightColor = pDirLight->GetLightColor();
	pCBShadowMap->lightViewMatrix = XMMatrixTranspose(lightViewMatrix);
	for (int nCascadeIndex = 0; nCascadeIndex < CASCADE_SHADOW_MAP_NUM; ++nCascadeIndex)
	{
		pCBShadowMap->arrayLightProjMatrix[nCascadeIndex] = 
			XMMatrixTranspose(mCascadeShadowManager->mArrayLightOrthoMatrix[nCascadeIndex]);
	}

	for (int index = 0; index < 3; ++index)
	{
		pCBShadowMap->lightCameraZ[index] = mCascadeShadowManager->mfCameraZ[index];
	}
	pCBShadowMap->shadowBias = 0.003f;

	g_pDeviceContext->Unmap(mCBShadowMap, 0);


	
	ID3D11ShaderResourceView* cascadeDirShadowMap = mCascadeShadowManager->GetShadowMapSRV();

	//����,������VertexShader�ĳ��������ֵ(���Ÿ��µ�ֵ)
	g_pDeviceContext->VSSetConstantBuffers(0, 1, &mCBCommon);
	g_pDeviceContext->PSSetConstantBuffers(0, 1, &mCBCommon);
	g_pDeviceContext->PSSetConstantBuffers(1, 1, &mCBShadowMap);
	g_pDeviceContext->PSSetShaderResources(0, 1, &worldPosTex);
	g_pDeviceContext->PSSetShaderResources(5, 1, &cascadeDirShadowMap);

	return true;
}

void ShadowMapShader::SetShaderState()
{
	Shader_2D::SetShaderState();
	g_pDeviceContext->PSSetSamplers(2, 1, &mBorderLinearSample);
}

