#ifndef _SSR_SHADER_H
#define _SSR_SHADER_H


#include "2DShader.h"
#include "CommonConstantBufferStruct.h"
#include "Camera.h"
using namespace std;
using namespace DirectX;


class SSRShader: public Shader_2D
{
private:
	struct CBSSR
	{
		float farPlane;
		float nearPlane;
		XMFLOAT2  perspectiveValue;
	};

private:
	ID3D11Buffer* mCBCommon; 
	ID3D11Buffer* mCBSSR;

private:
	void CreateConstantBuffer();

protected:
	// Õ∑≈Shader
	void virtual ShutDown();

public:
	SSRShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	SSRShader(const SSRShader& other);
	virtual ~SSRShader();

public:
	bool SetShaderCB(const CXMMATRIX& worldMatrix, ID3D11ShaderResourceView* arraySRV[5],
		XMFLOAT2 perspectiveValue);
	bool SetShaderParams(const CXMMATRIX& worldMatrix, ID3D11ShaderResourceView* arraySRV[5],
		XMFLOAT2 perspectiveValue);
};
#endif 