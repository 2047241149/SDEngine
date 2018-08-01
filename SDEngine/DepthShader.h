#pragma once

#ifndef _DEPTH_SHADER_H
#define _DEPTH_SHADER_H

#include "CoreMini.h"
#include "Camera.h"
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

public:

	//变换矩阵常量缓存结构体
	struct CBMatrix
	{
		XMMATRIX mUIViewMatrix;
		XMMATRIX mUIOrthoMatrix;
		float farPlane;
		float nearPlane;
		XMFLOAT2 pad;
	};


public:
	DepthShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	DepthShader(const DepthShader& other);
	virtual ~DepthShader();

public:
	bool SetShaderCB(ID3D11ShaderResourceView* diffuseTexture);
	bool SetShaderState();
	bool SetShaderParams(ID3D11ShaderResourceView* diffuseTexture);

};
#endif 
