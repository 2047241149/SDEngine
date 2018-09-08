#ifndef _FXAA_SHADER_H
#define _FXAA_SHADER_H


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
	bool SetShaderCB(ID3D11ShaderResourceView* screenRT, float fScrreenWidth, float fScrreenHeight);
	bool SetShaderParams(ID3D11ShaderResourceView* screenRT, float fScrreenWidth, float fScrreenHeight);

private:
	void CreateConstantBuffer();
	void CreateSamplerState();

protected:
	virtual void  ShutDown() override;
	virtual void SetShaderState() override;


private:
	struct CBFXAA
	{
		XMFLOAT4 rcpFrame;
	};

private:
	ID3D11Buffer* mCBFxaa;
	ID3D11SamplerState* mAnisotropicSampler;
};
#endif 