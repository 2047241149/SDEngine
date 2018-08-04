#pragma once
#ifndef _Wave_SHADER_H
#define _Wave_SHADER_H

#include "3DShader.h"

class WaveShader:public Shader_3D
{



public:
	WaveShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	WaveShader(const WaveShader& other);
	virtual ~WaveShader();

public:
	bool SetShaderCBExtern(CXMMATRIX worldMatrix, FXMVECTOR surfaceColor,ID3D11ShaderResourceView* diffuse, ID3D11ShaderResourceView* normal);
	bool SetShaderParamsExtern(CXMMATRIX worldMatrix, FXMVECTOR surfaceColor, ID3D11ShaderResourceView* diffuse, ID3D11ShaderResourceView* normal);

private:
	void CreateBuffer();


protected:
	virtual void ShutDown() override;

private:
	ID3D11Buffer* mCBEveryFrameBuffer;
private:
	//灯光常量缓存结构体
	struct CBEveryFrame
	{
		XMFLOAT4 surfaceColor;
	};

};
#endif 
