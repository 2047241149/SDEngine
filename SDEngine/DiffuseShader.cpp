#include"DiffuseShader.h"


DiffuseShader::DiffuseShader(WCHAR* vsFilenPath, WCHAR* psFilenPath):Shader(vsFilenPath, psFilenPath)
{
	CreateBuffer();
}


DiffuseShader::DiffuseShader(const DiffuseShader& other) :Shader(other)
{

}


DiffuseShader::~DiffuseShader()
{
	ShutDown();
}

void DiffuseShader::ShutDown()
{

}

void DiffuseShader::CreateBuffer()
{

}

bool DiffuseShader::SetShaderCBExtern(CXMMATRIX worldMatrix, ID3D11ShaderResourceView* diffuseTexture)
{
	ID3D11DeviceContext* d3dDeviceContext = D3DClass::GetInstance()->GetDeviceContext();
	Shader::SetShaderCB(worldMatrix);
	d3dDeviceContext->PSSetShaderResources(0, 1, &diffuseTexture);

	return true;
}


bool DiffuseShader::SetShaderParamsExtern(CXMMATRIX worldMatrix, ID3D11ShaderResourceView* diffuseTexture)
{

	bool result;

	//����Shader���������������Դ
	result = SetShaderCBExtern(worldMatrix, diffuseTexture);
	if (!result)
		return false;

	//����VertexShader PixelShader InputLayout SamplerState
	SetShaderState();

	return true;
}