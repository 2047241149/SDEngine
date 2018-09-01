#include "DefferedPointLightShader.h"

DefferedPointLightShader::DefferedPointLightShader(WCHAR* vsFilenPath, WCHAR* psFilenPath):
	Shader_3D(vsFilenPath,psFilenPath),
	mCBCommon(nullptr),
	m_pCBPointLight(nullptr)
{
	CreateConstantBuffer();
}


DefferedPointLightShader::DefferedPointLightShader(const DefferedPointLightShader& other):
	Shader_3D(other)
{

}


DefferedPointLightShader::~DefferedPointLightShader()
{
	ShutDown();
}

bool DefferedPointLightShader::SetShaderParams(ID3D11ShaderResourceView* gBuffer[3],int nPointLightIndex)
{
	bool result;
	//设置Shader常量缓存和纹理资源
	result = SetShaderCB(gBuffer, nPointLightIndex);
	if (!result)
		return false;

	//设置VertexShader PixelShader InputLayout SamplerState
	SetShaderState();

	return true;
}

void DefferedPointLightShader::CreateConstantBuffer()
{

	D3D11_BUFFER_DESC commonBufferDesc;
	ZeroMemory(&commonBufferDesc, sizeof(commonBufferDesc));
	commonBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	commonBufferDesc.ByteWidth = sizeof(CBCommmon);   
	commonBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	commonBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	g_pDevice->CreateBuffer(&commonBufferDesc, NULL, &mCBCommon);

	D3D11_BUFFER_DESC dirlightBufferDesc;
	ZeroMemory(&dirlightBufferDesc, sizeof(dirlightBufferDesc));
	dirlightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	dirlightBufferDesc.ByteWidth = sizeof(CBPointLight);
	dirlightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	dirlightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	g_pDevice->CreateBuffer(&dirlightBufferDesc, NULL, &m_pCBPointLight);
}



void DefferedPointLightShader::ShutDown()
{
	ReleaseCOM(mCBCommon);
	ReleaseCOM(m_pCBPointLight);
}


bool DefferedPointLightShader::SetShaderCB(ID3D11ShaderResourceView* gBuffer[3],int nPointLightIndex)
{
	XMMATRIX viewMatrix = GCamera->GetViewMatrix();
	XMMATRIX ProjMatrix = GCamera->GetProjectionMatrix();
	shared_ptr<PointLight> pPointLight = GLightManager->m_vecPointLight[nPointLightIndex];

	//第一，更新变换矩阵常量缓存的值
	//将矩阵转置,在传入常量缓存前进行转置,因为GPU对矩阵数据会自动进行一次转置
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	HR(g_pDeviceContext->Map(mCBCommon, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource));
	auto pCBCommon = reinterpret_cast<CBCommmon*>(mappedSubresource.pData);
	XMMATRIX worldMa = XMMatrixTranspose(pPointLight->GetWorldMatrix());
	XMMATRIX viewMa = XMMatrixTranspose(viewMatrix);
	XMMATRIX ProjMa = XMMatrixTranspose(ProjMatrix);
	pCBCommon->mWorldMatrix = worldMa;
	pCBCommon->mViewMatrix = viewMa;
	pCBCommon->mProjMatrix = ProjMa;
	pCBCommon->mWorldInvTranposeMatirx = XMMatrixTranspose(MathTool::GetInvenseTranspose(pPointLight->GetWorldMatrix()));
	pCBCommon->cameraPos = GCamera->GetPosition();
	g_pDeviceContext->Unmap(mCBCommon, 0);

	D3D11_MAPPED_SUBRESOURCE mappedSSLight;
	HR(g_pDeviceContext->Map(m_pCBPointLight, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSSLight));
	auto pCBPointLght = reinterpret_cast<CBPointLight*>(mappedSSLight.pData);

	XMFLOAT3 lightColor = pPointLight->GetLightColor();

	pCBPointLght->lightColor = XMFLOAT4(lightColor.x, lightColor.y, lightColor.z, pPointLight->GetLightIntensity());
	pCBPointLght->lightPos = pPointLight->GetPosition();
	pCBPointLght->radius = pPointLight->GetRadius();
	pCBPointLght->attenuation = pPointLight->GetLightAttenuation();
	g_pDeviceContext->Unmap(m_pCBPointLight, 0);

	//第三,设置在VertexShader的常量缓存的值(带着更新的值)
	g_pDeviceContext->VSSetConstantBuffers(0, 1, &mCBCommon);
	g_pDeviceContext->PSSetConstantBuffers(0, 1, &mCBCommon);
	g_pDeviceContext->PSSetConstantBuffers(1, 1, &m_pCBPointLight);
	g_pDeviceContext->PSSetShaderResources(0, 3, gBuffer);

	return true;
}



