#pragma once


#ifndef _DIFFUSE_NORMAL_SPEC_SHADER_H
#define _DIFFUSE_NORMAL_SPEC_SHADER_H
#include "3DShader.h"

class DiffuseNormalSpecShader:public Shader_3D
{

private:
	virtual void CreateBuffer();

protected:
	virtual void ShutDown();
	

public:
	DiffuseNormalSpecShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	DiffuseNormalSpecShader(const DiffuseNormalSpecShader&);
	~DiffuseNormalSpecShader();

public:
	bool SetShaderCBExtern(CXMMATRIX worldMatrix,
		ID3D11ShaderResourceView* diffuseSRV, ID3D11ShaderResourceView* normalSRV, ID3D11ShaderResourceView* specSRV);
	bool SetShaderParamsExtern(CXMMATRIX worldMatrix,
		ID3D11ShaderResourceView* diffuseSRV, ID3D11ShaderResourceView* normalSRV, ID3D11ShaderResourceView* specSRV);
	
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
	ID3D11Buffer* mCBLightBuffer;  //灯光常量缓存
	ID3D11Buffer* mCBCamera;  //灯光常量缓存

};
#endif 
