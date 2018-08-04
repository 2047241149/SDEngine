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
	//����Shader���������������Դ
	result = SetShaderCB(gBuffer);
	if (!result)
		return false;

	//����VertexShader PixelShader InputLayout SamplerState
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
	pCBCommon->dirLightColor = GLightManager->GetMainLight()->GetLightColor();
	pCBCommon->dirLightDir = GLightManager->GetMainLight()->GetLightDirection();
	pCBCommon->ambientLight = GLightManager->GetMainLight()->GetAmbientLight();
	pCBCommon->cameraPos = GCamera->GetPosition();
	g_pDeviceContext->Unmap(mCBCommon, 0);
	//����,������VertexShader�ĳ��������ֵ(���Ÿ��µ�ֵ)
	g_pDeviceContext->VSSetConstantBuffers(0, 1, &mCBCommon);
	g_pDeviceContext->PSSetConstantBuffers(0, 1, &mCBCommon);
	g_pDeviceContext->PSSetShaderResources(0, 4, gBuffer);

	return true;
}



