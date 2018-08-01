#ifndef _SSR_GBUFFER_SHADER_H
#define _SSR_GBUFFER_SHADER_H

#include<Windows.h>
#include"Macro.h"
#include<d3d11_1.h>
#include<d3dcompiler.h>
#include<DirectXMath.h>
#include<fstream>
#include"MathTool.h"
#include"DirectxCore.h"
#include"Shader.h"
#include"Camera.h"
#include"Light.h"
#include "CommonConstantBufferStruct.h"
#include "Log.h"
using namespace std;
using namespace DirectX;


class SSRGBufferShader
{
private:

	ID3D11VertexShader* md3dVertexShader;
	ID3D11PixelShader* md3dPixelShader;
	ID3D11InputLayout* md3dInputLayout; 
	ID3D11Buffer* mCBCommon; 
	ID3D11SamplerState *mSamplerLinearWrap;
	ID3D11SamplerState *mSamplerLinearClamp;

public:
	bool virtual Init(WCHAR* vsFilenPath, WCHAR* psFilenPath);

	//��ʼ��Shader,���ڴ���InputLayout,VertexShader,PixelShader,��������
	bool virtual InitializeShader(WCHAR*, WCHAR*);

	//�ͷ�Shader
	void virtual ShutDown();


public:
	SSRGBufferShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	SSRGBufferShader(const SSRGBufferShader& other);
	virtual ~SSRGBufferShader();

public:
	bool SetShaderCB(ID3D11ShaderResourceView* gBuffer[2]);
	bool SetShaderState();
	bool SetShaderParams(ID3D11ShaderResourceView* gBuffer[2]);
};
#endif 