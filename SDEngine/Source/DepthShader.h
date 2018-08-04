#pragma once

#ifndef _DEPTH_SHADER_H
#define _DEPTH_SHADER_H

#include "CoreMini.h"
#include "Camera.h"
#include "2DShader.h"
using namespace std;
using namespace DirectX;


class DepthShader:public Shader_2D
{
private:
	ID3D11Buffer* mCBMatrixBuffer; //变换矩阵(常量)缓存,顶点索引

private:
	void CreateConstantBuffer();

protected:

	void virtual ShutDown() override;

public:
	DepthShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	DepthShader(const DepthShader& other);
	virtual ~DepthShader();

public:
	bool SetShaderCB(ID3D11ShaderResourceView* diffuseTexture);
	bool SetShaderParams(ID3D11ShaderResourceView* diffuseTexture);

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
};





#endif 
