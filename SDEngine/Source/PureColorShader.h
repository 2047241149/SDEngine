#pragma once
#ifndef _PURE_COLOR_SHADER_H
#define _PURE_COLOR_SHADER_H

#include "3DShader.h"

class PureColorShader:public Shader_3D
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
	virtual void CreateConstantBuffer();

protected:
	virtual void ShutDown() override;

public:
	PureColorShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	PureColorShader(const PureColorShader& other);
	virtual ~PureColorShader();

public:
	bool SetShaderCBExtern(CXMMATRIX worldMatrix, FXMVECTOR surfaceColor);
	bool SetShaderParamsExtern(CXMMATRIX worldMatrix, FXMVECTOR surfaceColor);
};
#endif 
