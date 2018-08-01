#pragma once

#ifndef _UI_SHADER_H
#define _UI_SHADER_H
#include<Windows.h>
#include"Macro.h"
#include<d3d11_1.h>
#include<d3dcompiler.h>
#include<DirectXMath.h>
#include<fstream>
#include"MathTool.h"
#include "DirectxCore.h"
#include"Camera.h"
#include "Log.h"
using namespace std;
using namespace DirectX;


class UIShader
{
private:

	ID3D11VertexShader* md3dVertexShader;
	ID3D11PixelShader* md3dPixelShader;
	ID3D11InputLayout* md3dInputLayout;
	ID3D11Buffer* mCBMatrixBuffer; //变换矩阵(常量)缓存,顶点索引
	ID3D11SamplerState *mSamplerState; //采样状态(采样方式)

public:
	bool virtual Init(WCHAR* vsFilenPath, WCHAR* psFilenPath);

	//初始化Shader,用于创建InputLayout,VertexShader,PixelShader,常量缓存
	bool virtual InitializeShader(WCHAR*, WCHAR*);

	//释放Shader
	void virtual ShutDown();

public:

	//变换矩阵常量缓存结构体
	struct CBMatrix
	{
		XMMATRIX mUIViewMatrix;
		XMMATRIX mUIOrthoMatrix;
	};


public:
	UIShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	UIShader(const UIShader& other);
	virtual ~UIShader();

public:
	bool SetShaderCB(ID3D11ShaderResourceView* diffuseTexture);
	bool SetShaderState();
	bool SetShaderParams(ID3D11ShaderResourceView* diffuseTexture);

};
#endif 
