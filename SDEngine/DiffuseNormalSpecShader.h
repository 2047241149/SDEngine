#pragma once


#ifndef _DIFFUSE_NORMAL_SPEC_SHADER_H
#define _DIFFUSE_NORMAL_SPEC_SHADER_H
#include"Shader.h"

class DiffuseNormalSpecShader:public Shader
{
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

private:
	virtual void ShutDown();
	virtual void CreateBuffer();

private:
	ID3D11Buffer* mCBLightBuffer;  //灯光常量缓存
	ID3D11Buffer* mCBCamera;  //灯光常量缓存

public:
	DiffuseNormalSpecShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	DiffuseNormalSpecShader(const DiffuseNormalSpecShader&);
	~DiffuseNormalSpecShader();

public:
	bool SetShaderCBExtern(CXMMATRIX worldMatrix,
		ID3D11ShaderResourceView* diffuseSRV, ID3D11ShaderResourceView* normalSRV, ID3D11ShaderResourceView* specSRV);
	bool SetShaderParamsExtern(CXMMATRIX worldMatrix,
		ID3D11ShaderResourceView* diffuseSRV, ID3D11ShaderResourceView* normalSRV, ID3D11ShaderResourceView* specSRV);
	
};
#endif 
