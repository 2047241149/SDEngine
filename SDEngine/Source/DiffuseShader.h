#pragma once


#ifndef _DIFFUSE_SHADER_H
#define _DIFFUSE_SHADER_H
#include "3DShader.h"

class DiffuseShader : public Shader_3D
{


protected:
	virtual void ShutDown() override;

public:
	DiffuseShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	DiffuseShader(const DiffuseShader&);
	~DiffuseShader();

public:
	bool SetShaderCBExtern(CXMMATRIX worldMatrix, ID3D11ShaderResourceView* diffuseTexture);
	bool SetShaderParamsExtern(CXMMATRIX worldMatrix, ID3D11ShaderResourceView* diffuseTexture);
	
};
#endif 
