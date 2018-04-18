#pragma once
#ifndef _PURE_COLOR_SHADER_H
#define _PURE_COLOR_SHADER_H

#include"Shader.h"

class PureColorShader:Shader
{
private:
	ID3D11Buffer* mCBEveryFrameBuffer;
private:
	//灯光常量缓存结构体
	struct CBEveryFrame
	{
		XMFLOAT4 surfaceColor;
	};

private:
	virtual void ShutDown();
	virtual void CreateBuffer();

public:
	PureColorShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	PureColorShader(const PureColorShader& other);
	virtual ~PureColorShader();

public:
	bool SetShaderCBExtern(CXMMATRIX worldMatrix, FXMVECTOR surfaceColor);
	bool SetShaderParamsExtern(CXMMATRIX worldMatrix, FXMVECTOR surfaceColor);
};
#endif 
