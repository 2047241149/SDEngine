#pragma once
#ifndef _Wave_SHADER_H
#define _Wave_SHADER_H

#include"Shader.h"

class WaveShader:public Shader
{
private:
	ID3D11Buffer* mCBEveryFrameBuffer;
private:
	//�ƹⳣ������ṹ��
	struct CBEveryFrame
	{
		XMFLOAT4 surfaceColor;
	};

private:
	virtual void ShutDown();
	virtual void CreateBuffer();

public:
	WaveShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	WaveShader(const WaveShader& other);
	virtual ~WaveShader();

public:
	bool SetShaderCBExtern(CXMMATRIX worldMatrix, FXMVECTOR surfaceColor,ID3D11ShaderResourceView* diffuse, ID3D11ShaderResourceView* normal);
	bool SetShaderParamsExtern(CXMMATRIX worldMatrix, FXMVECTOR surfaceColor, ID3D11ShaderResourceView* diffuse, ID3D11ShaderResourceView* normal);
};
#endif 
