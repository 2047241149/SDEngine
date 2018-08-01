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

	//��ʼ��Shader,���ڴ���InputLayout,VertexShader,PixelShader,��������
	bool virtual InitializeShader(WCHAR*, WCHAR*);

	//�ͷ�Shader
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