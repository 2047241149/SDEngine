#pragma once
#ifndef _DEFFERED_FINAL_SHADER_H
#define _DEFFERED_FINAL_SHADER_H

#include "CoreMini.h"
#include"Camera.h"
#include "CommonConstantBufferStruct.h"
#include "2DShader.h"
using namespace std;
using namespace DirectX;


class DefferedFinalShader : public Shader_2D
{

public:
	DefferedFinalShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	DefferedFinalShader(const DefferedFinalShader& other);
	virtual ~DefferedFinalShader();

public:
	bool SetShaderCB(ID3D11ShaderResourceView* gBuffer[2]);
	bool SetShaderParams(ID3D11ShaderResourceView* gBuffer[2]);
};
#endif 