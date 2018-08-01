#ifndef _GRAPHICS_BLIT_SHADER_H
#define _GRAPHICS_BLIT_SHADER_H

#include "CoreMini.h"
#include"Shader.h"
#include"Camera.h"
#include"Light.h"
#include "CommonConstantBufferStruct.h"
using namespace std;
using namespace DirectX;


class GraphcisBlitShader
{
private:

	ID3D11VertexShader* md3dVertexShader;
	ID3D11PixelShader* md3dPixelShader;
	ID3D11InputLayout* md3dInputLayout;  
	ID3D11SamplerState *mSamplerLinearWrap;
	ID3D11SamplerState *mSamplerLinearClamp;

public:
	bool virtual Init(WCHAR* vsFilenPath, WCHAR* psFilenPath);

	//初始化Shader,用于创建InputLayout,VertexShader,PixelShader,常量缓存
	bool virtual InitShader(WCHAR*, WCHAR*);

	//释放Shader
	void virtual ShutDown();

public:
	GraphcisBlitShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	GraphcisBlitShader(const GraphcisBlitShader& other);
	virtual ~GraphcisBlitShader();

public:
	bool SetShaderCB(ID3D11ShaderResourceView* screenRT);
	bool SetShaderState();
	bool SetShaderParams(ID3D11ShaderResourceView* screenRT);
};
#endif 