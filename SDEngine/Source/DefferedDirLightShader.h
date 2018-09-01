#ifndef _DEFFER_DIR_LIGHT_SHADER_H
#define _DEFFER_DIR_LIGHT_SHADER_H

#include "CoreMini.h"
#include"Camera.h"
#include "CommonConstantBufferStruct.h"
#include "2DShader.h"
using namespace std;
using namespace DirectX;


class DefferedDirLightShader : public Shader_2D
{
private:
	ID3D11Buffer* mCBCommon; 
	ID3D11Buffer* m_pCBDirLight;

private:
	void CreateConstantBuffer();

protected:
	virtual void  ShutDown() override;

public:
	DefferedDirLightShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	DefferedDirLightShader(const DefferedDirLightShader& other);
	virtual ~DefferedDirLightShader();

public:
	bool SetShaderCB(ID3D11ShaderResourceView* gBuffer[3], int nDirLightIndex);
	bool SetShaderParams(ID3D11ShaderResourceView* gBuffer[3], int nDirLightIndex);
};
#endif 