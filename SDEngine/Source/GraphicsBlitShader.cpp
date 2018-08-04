#include"GraphicsBlitShader.h"

GraphcisBlitShader::GraphcisBlitShader(WCHAR* vsFilenPath, WCHAR* psFilenPath):
	Shader_2D(vsFilenPath,psFilenPath)
{

}


GraphcisBlitShader::GraphcisBlitShader(const GraphcisBlitShader& other):
	Shader_2D(other)
{

}


GraphcisBlitShader::~GraphcisBlitShader()
{
	
}



bool GraphcisBlitShader::SetShaderParams(ID3D11ShaderResourceView* screenRT)
{
	bool result;
	//����Shader���������������Դ
	result = SetShaderCB(screenRT);
	if (!result)
		return false;

	//����VertexShader PixelShader InputLayout SamplerState
	SetShaderState();

	return true;
}


bool GraphcisBlitShader::SetShaderCB(ID3D11ShaderResourceView* screenRT)
{
	g_pDeviceContext->PSSetShaderResources(0, 1, &screenRT);
	return true;
}



