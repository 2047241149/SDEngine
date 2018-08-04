#include"DepthShader.h"

DepthShader::DepthShader(WCHAR* vsFilenPath, WCHAR* psFilenPath):
	Shader_2D(vsFilenPath,psFilenPath)
{
	CreateConstantBuffer();
}


DepthShader::DepthShader(const DepthShader& other):
	Shader_2D(other)
{

}


DepthShader::~DepthShader()
{
	ShutDown();
}





bool DepthShader::SetShaderParams(ID3D11ShaderResourceView* diffuseTexture)
{
	bool result;

	//����Shader���������������Դ
	result = SetShaderCB(diffuseTexture);
	if (!result)
		return false;

	//����VertexShader PixelShader InputLayout SamplerState
	SetShaderState();

	return true;
}




void DepthShader::CreateConstantBuffer()
{

	//����(�任������)�������ݽṹ��,����������������
	D3D11_BUFFER_DESC matrixBufferDesc;
	ZeroMemory(&matrixBufferDesc, sizeof(matrixBufferDesc));
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(CBMatrix);  
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	g_pDevice->CreateBuffer(&matrixBufferDesc, NULL, &mCBMatrixBuffer);
}



void DepthShader::ShutDown()
{
	ReleaseCOM(mCBMatrixBuffer);
}


bool DepthShader::SetShaderCB(ID3D11ShaderResourceView* diffuseTexture)
{

	XMMATRIX uiViewMatrix = GCamera->GetUIViewMatrix();
	XMMATRIX uiOrhoMatrix = GCamera->GetUIOrthoMatrix();
	//��һ�����±任�����������ֵ
	//������ת��,�ڴ��볣������ǰ����ת��,��ΪGPU�Ծ������ݻ��Զ�����һ��ת��
	D3D11_MAPPED_SUBRESOURCE mappedSubresource1;
	HR(g_pDeviceContext->Map(mCBMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource1));
	auto pCBMatrix = reinterpret_cast<CBMatrix*>(mappedSubresource1.pData);
	XMMATRIX uiViewMa = XMMatrixTranspose(uiViewMatrix);
	XMMATRIX uiOrthoMa = XMMatrixTranspose(uiOrhoMatrix);
	pCBMatrix->mUIViewMatrix = uiViewMa;
	pCBMatrix->mUIOrthoMatrix = uiOrthoMa;
	pCBMatrix->farPlane =GCamera->GetFarPlane();
	pCBMatrix->nearPlane = GCamera->GetNearPlane();
	pCBMatrix->pad = XMFLOAT2(0.0f, 0.0f);
	g_pDeviceContext->Unmap(mCBMatrixBuffer, 0);


	//����,������VertexShader�ĳ��������ֵ(���Ÿ��µ�ֵ)
	g_pDeviceContext->VSSetConstantBuffers(0, 1, &mCBMatrixBuffer);
	g_pDeviceContext->PSSetConstantBuffers(0, 1, &mCBMatrixBuffer);
	g_pDeviceContext->PSSetShaderResources(0, 1, &diffuseTexture);

	return true;
}


