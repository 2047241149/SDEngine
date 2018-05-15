#pragma once

#ifndef _SSR_SHADER_H
#define _SSR_SHADER_H
#include"Shader.h"
#include "Camera.h"

class SSRShader:Shader
{
public:
	struct CBSSR
	{
		float farPlane;
		float nearPlane;
		XMFLOAT2  perspectiveValue;
	};

private:
	ID3D11Buffer* mCBSSR; 
private:
	virtual void ShutDown();
	virtual void CreateBuffer();
public:
	SSRShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	SSRShader(const SSRShader&);
	~SSRShader();

public:
	bool SetShaderCBExtern(CXMMATRIX worldMatrix, ID3D11ShaderResourceView* diffuseTex,
		ID3D11ShaderResourceView* frontDepthTex, ID3D11ShaderResourceView* backDepthTex, 
		XMFLOAT2 perspectiveValue);

	bool SetShaderParamsExtern(CXMMATRIX worldMatrix, ID3D11ShaderResourceView* diffuseTex, 
		ID3D11ShaderResourceView* frontDepthTex, ID3D11ShaderResourceView* backDepthTex, 
		XMFLOAT2 perspectiveValue);
	
};
#endif 
