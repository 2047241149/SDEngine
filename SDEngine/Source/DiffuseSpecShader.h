#pragma once


#ifndef _DIFFUSE_SPEC_SHADER_H
#define _DIFFUSE_SPEC_SHADER_H
#include "3DShader.h"

class DiffuseSpecShader:public Shader_3D
{

public:
	DiffuseSpecShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	DiffuseSpecShader(const DiffuseSpecShader&);
	~DiffuseSpecShader();

public:
	bool SetShaderCBExtern(CXMMATRIX worldMatrix,
		ID3D11ShaderResourceView* diffuseSRV, ID3D11ShaderResourceView* specSRV);
	bool SetShaderParamsExtern(CXMMATRIX worldMatrix,
		ID3D11ShaderResourceView* diffuseSRV, ID3D11ShaderResourceView* specSRV);

protected:
	virtual void ShutDown();

private:
	void CreateBuffer();


private:
	ID3D11Buffer* mCBLightBuffer;  //灯光常量缓存
	ID3D11Buffer* mCBCamera;  //灯光常量缓存

private:

	struct CBLight
	{
		XMFLOAT4 lightColor;
		XMFLOAT3 lightDireciton;
		float pad;
	};

	struct CBCamera
	{
		XMFLOAT3 cameraPos;
		float pad;
	};
};
#endif 
