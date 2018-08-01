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
	ID3D11Buffer* mCBMatrixBuffer; //�任����(����)����,��������
	ID3D11SamplerState *mSamplerState; //����״̬(������ʽ)

public:
	bool virtual Initialize(WCHAR* vsFilenPath, WCHAR* psFilenPath);

	//��ʼ��Shader,���ڴ���InputLayout,VertexShader,PixelShader,��������
	bool virtual InitializeShader(WCHAR*, WCHAR*);

	//�ͷ�Shader
	void virtual ShutDown();

public:

	//�任����������ṹ��
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
