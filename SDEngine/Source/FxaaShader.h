#ifndef _FXAA_SHADER_H
#define _FXAA_SHADER_H

#include 
#include "CommonConstantBufferStruct.h"
#include "2DShader.h"
using namespace std;
using namespace DirectX;


class FxaaShader:public Shader_2D
{

public:
	FxaaShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	FxaaShader(const FxaaShader& other);
	virtual ~FxaaShader();

public:
	bool SetShaderCB(ID3D11ShaderResourceView* screenRT, ID3D11ShaderResourceView* screenBlurRT, 
		ID3D11ShaderResourceView* depthRT,
		float dofStart,float dofRange, float farPlane, float nearPlane);
	bool SetShaderParams(ID3D11ShaderResourceView* screenRT, ID3D11ShaderResourceView* screenBlurRT,
		ID3D11ShaderResourceView* depthRT,
		float dofStart, float dofRange,float farPlane, float nearPlane);

private:
	void CreateConstantBuffer();

protected:
	virtual void  ShutDown() override;



private:
	struct CBFXAA
	{

	};

private:
	ID3D11Buffer* mCBFxaa;
};
#endif 