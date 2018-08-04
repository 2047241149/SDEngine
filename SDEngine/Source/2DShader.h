#pragma once
#ifndef _2D_SHADER_H
#define _2D_SHADER_H
#include "CoreMini.h"
class Shader_2D
{

public:
	Shader_2D(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	Shader_2D(const Shader_2D& other);
	virtual ~Shader_2D();

public:
	virtual bool  Init(WCHAR* vsFilenPath, WCHAR* psFilenPath);

	//初始化Shader,用于创建InputLayout,VertexShader,PixelShader
	virtual bool  InitShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);

protected:
	//释放Shader
	virtual void  ShutDown();

public:
	bool SetShaderCB(ID3D11ShaderResourceView* screenRT);
	bool SetShaderState();
	bool SetShaderParams(ID3D11ShaderResourceView* screenRT);

private:
	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;
	ID3D11InputLayout* m_pInputLayout;
	ID3D11SamplerState *m_pWrapLinearSampler;
	ID3D11SamplerState *m_pClampPointSampler;
};
#endif