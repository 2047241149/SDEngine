#ifndef _DEFFER_LIGHTING_SHADER_H
#define _DEFFER_LIGHTING_SHADER_H

#include "CoreMini.h"
#include"Camera.h"
#include "CommonConstantBufferStruct.h"
#include "2DShader.h"
using namespace std;
using namespace DirectX;


class DefferLighingShader : public Shader_2D
{
private:
	ID3D11Buffer* mCBCommon; 


private:
	void CreateConstantBuffer();

protected:
	virtual void  ShutDown() override;

public:
	DefferLighingShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	DefferLighingShader(const DefferLighingShader& other);
	virtual ~DefferLighingShader();


public:
	bool SetShaderCB(ID3D11ShaderResourceView* gBuffer[4]);
	bool SetShaderParams(ID3D11ShaderResourceView* gBuffer[4]);
};
#endif 