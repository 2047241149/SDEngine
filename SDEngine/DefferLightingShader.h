#ifndef _DEFFER_LIGHTING_SHADER_H
#define _DEFFER_LIGHTING_SHADER_H

#include "CoreMini.h"
#include"Camera.h"
#include "CommonConstantBufferStruct.h"
using namespace std;
using namespace DirectX;


class DefferLighingShader
{
private:

	ID3D11VertexShader* md3dVertexShader;
	ID3D11PixelShader* md3dPixelShader;
	ID3D11InputLayout* md3dInputLayout; 
	ID3D11Buffer* mCBCommon; 
	ID3D11SamplerState *mSamplerLinearWrap;
	ID3D11SamplerState *mSamplerLinearClamp;

public:
	bool virtual Initialize(WCHAR* vsFilenPath, WCHAR* psFilenPath);

	//初始化Shader,用于创建InputLayout,VertexShader,PixelShader,常量缓存
	bool virtual InitializeShader(WCHAR*, WCHAR*);

	//释放Shader
	void virtual ShutDown();

public:
	DefferLighingShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	DefferLighingShader(const DefferLighingShader& other);
	virtual ~DefferLighingShader();

public:
	bool SetShaderCB(ID3D11ShaderResourceView* gBuffer[4]);
	bool SetShaderState();
	bool SetShaderParams(ID3D11ShaderResourceView* gBuffer[4]);
};
#endif 