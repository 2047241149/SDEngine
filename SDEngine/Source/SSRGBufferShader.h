#ifndef _SSR_GBUFFER_SHADER_H
#define _SSR_GBUFFER_SHADER_H

#include "2DShader.h"
#include "Camera.h"
using namespace std;
using namespace DirectX;


class SSRGBufferShader: public Shader_2D
{

private:
	ID3D11Buffer* mCBCommon; 

protected:
	virtual void  ShutDown() override;

private:
	void CreateConstantBuffer();

public:
	SSRGBufferShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	SSRGBufferShader(const SSRGBufferShader& other);
	virtual ~SSRGBufferShader();

public:
	bool SetShaderCB(ID3D11ShaderResourceView* gBuffer[2]);
	bool SetShaderParams(ID3D11ShaderResourceView* gBuffer[2]);
};
#endif 