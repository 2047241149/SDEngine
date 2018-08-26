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
	//设置Shader常量缓存和纹理资源
	result = SetShaderCB(gBuffer);
	if (!result)
		return false;

	//设置VertexShader PixelShader InputLayout SamplerState
	SetShaderState();

	return true;
}




bool DefferedFinalShader::SetShaderCB(ID3D11ShaderResourceView* gBuffer[2])
{

	g_pDeviceContext->PSSetShaderResources(0, 2, gBuffer);
	return true;
}

