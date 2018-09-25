#pragma once
#ifndef _LIGHT_DEPTH_MAP_SHADER_H
#define _LIGHT_DEPTH_MAP_SHADER_H

#include "3DShader.h"

class LightDepthShader : public Shader_3D
{

public:
	LightDepthShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	LightDepthShader(const LightDepthShader& other);
	virtual ~LightDepthShader();

public:
	bool SetShaderParamsExtern(CXMMATRIX worldMatrix, CXMMATRIX lightOrthoProjMatrix);
	bool SetShaderCBExtern(CXMMATRIX worldMatrix, CXMMATRIX lightOrthoProjMatrix);
};
#endif 
