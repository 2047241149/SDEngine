#include "DefferedFinalShader.h"



DefferedFinalShader::DefferedFinalShader(WCHAR* vsFilenPath, WCHAR* psFilenPath) :
	Shader_2D(vsFilenPath, psFilenPath)
{

}


DefferedFinalShader::DefferedFinalShader(const DefferedFinalShader& other) :
	Shader_2D(other)
{

}


DefferedFinalShader::~DefferedFinalShader()
{
}




bool DefferedFinalShader::SetShaderParams(ID3D11ShaderResourceView* gBuffer[2])
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




bool DefferedFinalShader::SetShaderCB(ID3D11ShaderResourceView* gBuffer[2])
{

	g_pDeviceContext->PSSetShaderResources(0, 2, gBuffer);
	return true;
}

