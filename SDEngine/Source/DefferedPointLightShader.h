#ifndef _DEFFER_POINT_LIGHT_SHADER_H
#define _DEFFER_POINT_LIGHT_SHADER_H

#include "CoreMini.h"
#include"Camera.h"
#include "CommonConstantBufferStruct.h"
#include "3DShader.h"
using namespace std;
using namespace DirectX;


class DefferedPointLightShader : public Shader_3D
{
private:
	ID3D11Buffer* mCBCommon; 
	ID3D11Buffer* m_pCBPointLight;

private:
	void CreateConstantBuffer();

protected:
	virtual void  ShutDown() override;

public:
	DefferedPointLightShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	DefferedPointLightShader(const DefferedPointLightShader& other);
	virtual ~DefferedPointLightShader();

public:
	bool SetShaderCB(ID3D11ShaderResourceView* gBuffer[3],int nPointLightIndex);
	bool SetShaderParams(ID3D11ShaderResourceView* gBuffer[3], int nPointLightIndex);
};
#endif 