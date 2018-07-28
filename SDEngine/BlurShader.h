#ifndef _BLUR_SHADER_H
#define _BLUR_SHADER_H

#include<Windows.h>
#include"Macro.h"
#include<d3d11_1.h>
#include<d3dcompiler.h>
#include<DirectXMath.h>
#include<fstream>
#include"MathTool.h"
#include"D3DClass.h"
#include"Shader.h"
#include"Camera.h"
#include"Light.h"
#include "CommonConstantBufferStruct.h"
using namespace std;
using namespace DirectX;


class BlurShader
{
private:

	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;
	ID3D11InputLayout* m_pInputLayout;
	ID3D11Buffer* m_pCBCommon;
	ID3D11SamplerState *m_pWrapLinearSampler;
	ID3D11SamplerState *m_pClampPointSampler;

public:
	bool virtual Init(WCHAR* vsFilenPath, WCHAR* psFilenPath);

	//初始化Shader,用于创建InputLayout,VertexShader,PixelShader,常量缓存
	bool virtual InitShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);

	//释放Shader
	void virtual ShutDown();

public:
	BlurShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	BlurShader(const BlurShader& other);
	virtual ~BlurShader();

public:
	bool SetShaderCB(ID3D11ShaderResourceView* screenRT);
	bool SetShaderState();
	bool SetShaderParams(ID3D11ShaderResourceView* screenRT);
};
#endif 