#pragma once


#ifndef _DIFFUSE_NORMAL_SHADER_H
#define _DIFFUSE_NORMAL_SHADER_H
#include"Shader.h"

class DiffuseNormalShader:Shader
{
private:
	//灯光常量缓存结构体
	struct CBLight
	{
		XMFLOAT4 lightColor;
		XMFLOAT3 lightDireciton;
		float pad;
	};

private:
	virtual void ShutDown();
	virtual void CreateBuffer();

private:
	ID3D11Buffer* mCBLightBuffer;  //灯光常量缓存

public:
	DiffuseNormalShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	DiffuseNormalShader(const DiffuseNormalShader&);
	~DiffuseNormalShader();

public:
	bool SetShaderCBExtern(CXMMATRIX worldMatrix,
		ID3D11ShaderResourceView* diffuseSRV, ID3D11ShaderResourceView* normalSRV);
	bool SetShaderParamsExtern(CXMMATRIX worldMatrix,ID3D11ShaderResourceView* diffuseSRV,
		ID3D11ShaderResourceView* normalSRV);
	

};
#endif 
