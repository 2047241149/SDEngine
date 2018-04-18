#pragma once

#ifndef _DEPTH_SHADER_H
#define _DEPTH_SHADER_H
#include<Windows.h>
#include"Macro.h"
#include<d3d11_1.h>
#include<d3dcompiler.h>
#include<DirectXMath.h>
#include<fstream>
#include"MathTool.h"
#include"D3DClass.h"
using namespace std;
using namespace DirectX;


class DepthShader
{
private:

	ID3D11VertexShader* md3dVertexShader;
	ID3D11PixelShader* md3dPixelShader;
	ID3D11InputLayout* md3dInputLayout;
	ID3D11Buffer* mCBMatrixBuffer; //变换矩阵(常量)缓存,顶点索引
	ID3D11SamplerState *mSamplerState; //采样状态(采样方式)

public:
	bool virtual Initialize(WCHAR* vsFilenPath, WCHAR* psFilenPath);

	//初始化Shader,用于创建InputLayout,VertexShader,PixelShader,常量缓存
	bool virtual InitializeShader(WCHAR*, WCHAR*);

	//释放Shader
	void virtual ShutDown();

	//输出Shader编译文件的错误信息
	void virtual OutputShaderErrorMessage(ID3D10Blob*, WCHAR*);

public:

	//变换矩阵常量缓存结构体
	struct CBMatrix
	{
		XMMATRIX mUIViewMatrix;
		XMMATRIX mUIOrthoMatrix;
	};


public:
	DepthShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	DepthShader(const DepthShader& other);
	virtual ~DepthShader();

public:
	bool SetShaderCB(CXMMATRIX uiViewMatrix, CXMMATRIX uiOrhoMatrix, 
		ID3D11ShaderResourceView* diffuseTexture);
	bool SetShaderState();
	bool SetShaderParams(CXMMATRIX uiViewMatrix, CXMMATRIX uiOrhoMatrix, 
		ID3D11ShaderResourceView* diffuseTexture);

};
#endif 
