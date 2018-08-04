#include "BlurShader.h"

BlurShader::BlurShader(WCHAR* vsFilenPath, WCHAR* psFilenPath):
	Shader_2D(vsFilenPath,psFilenPath)
{

	
}


BlurShader::BlurShader(const BlurShader& other):
	Shader_2D(other)
{

}


BlurShader::~BlurShader()
{
	
}


