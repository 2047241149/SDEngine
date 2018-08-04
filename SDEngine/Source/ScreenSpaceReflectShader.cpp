#include "ScreenSpaceReflectShader.h"

SSRShader::SSRShader(WCHAR* vsFilenPath, WCHAR* psFilenPath):
	Shader_2D(vsFilenPath,psFilenPath),
	mCBCommon(nullptr),
	mCBSSR(nullptr)

{
	
}


SSRShader::SSRShader(const SSRShader& other):
	Shader_2D(other)
{

}


SSRShader::~SSRShader()
{
	ShutDown();
}


bool SSRShader::SetShaderParams(const CXMMATRIX& worldMatrix, ID3D11ShaderResourceView* arraySRV[5],
	XMFLOAT2 perspectiveValue)
{
	bool result;
	//设置Shader常量缓存和纹理资源
	result = SetShaderCB(worldMatrix, arraySRV,perspectiveValue);
	if (!result)
		return false;

	//设置VertexShader PixelShader InputLayout SamplerState
	SetShaderState();

	return true;
}




void SSRShader::CreateConstantBuffer()
{

	D3D11_BUFFER_DESC commonBufferDesc;
	ZeroMemory(&commonBufferDesc, sizeof(commonBufferDesc));
	commonBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	commonBufferDesc.ByteWidth = sizeof(CBCommmon);   
	commonBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	commonBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	g_pDevice->CreateBuffer(&commonBufferDesc, NULL, &mCBCommon);


	D3D11_BUFFER_DESC ssrBufferDesc;
	ZeroMemory(&ssrBufferDesc, sizeof(ssrBufferDesc));
	ssrBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	ssrBufferDesc.ByteWidth = sizeof(CBSSR);   
	ssrBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ssrBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	g_pDevice->CreateBuffer(&ssrBufferDesc, NULL, &mCBSSR);

}



void SSRShader::ShutDown()
{
	ReleaseCOM(mCBCommon);
}


bool SSRShader::SetShaderCB(const CXMMATRIX& worldMatrix, ID3D11ShaderResourceView* arraySRV[5],
	XMFLOAT2 perspectiveValue)
{

	XMMATRIX viewMatrix = GCamera->GetViewMatrix();
	XMMATRIX ProjMatrix = GCamera->GetProjectionMatrix();

	//第一，更新变换矩阵常量缓存的值
	//将矩阵转置,在传入常量缓存前进行转置,因为GPU对矩阵数据会自动进行一次转置
	D3D11_MAPPED_SUBRESOURCE mappedSubresource1;
	HR(g_pDeviceContext->Map(mCBCommon, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource1));
	auto pCBCommon = reinterpret_cast<CBCommmon*>(mappedSubresource1.pData);
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


	D3D11_MAPPED_SUBRESOURCE mappedSubresource2;
	HR(g_pDeviceContext->Map(mCBSSR, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource2));
	auto pCBSSR = reinterpret_cast<CBSSR*>(mappedSubresource2.pData);
	pCBSSR->farPlane = GCamera->GetFarPlane();
	pCBSSR->nearPlane = GCamera->GetNearPlane();
	pCBSSR->perspectiveValue = perspectiveValue;
	g_pDeviceContext->Unmap(mCBSSR, 0);


	//第三,设置在VertexShader的常量缓存的值(带着更新的值)
	g_pDeviceContext->VSSetConstantBuffers(0, 1, &mCBCommon);
	g_pDeviceContext->PSSetConstantBuffers(0, 1, &mCBCommon);

	g_pDeviceContext->PSSetConstantBuffers(1, 1, &mCBSSR);

	g_pDeviceContext->PSSetShaderResources(0, 5, arraySRV);

	return true;
}

