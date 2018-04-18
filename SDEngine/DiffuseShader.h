#pragma once


#ifndef _DIFFUSE_SHADER_H
#define _DIFFUSE_SHADER_H
#include"Shader.h"

class DiffuseShader:Shader
{
private:
	virtual void ShutDown();
	virtual void CreateBuffer();
public:
	DiffuseShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	DiffuseShader(const DiffuseShader&);
	~DiffuseShader();

public:
	bool SetShaderCBExtern(CXMMATRIX worldMatrix, ID3D11ShaderResourceView* diffuseTexture);
	bool SetShaderParamsExtern(CXMMATRIX worldMatrix, ID3D11ShaderResourceView* diffuseTexture);
	
};
#endif 
