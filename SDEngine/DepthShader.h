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
	ID3D11Buffer* mCBMatrixBuffer; //�任����(����)����,��������
	ID3D11SamplerState *mSamplerState; //����״̬(������ʽ)

public:
	bool virtual Initialize(WCHAR* vsFilenPath, WCHAR* psFilenPath);

	//��ʼ��Shader,���ڴ���InputLayout,VertexShader,PixelShader,��������
	bool virtual InitializeShader(WCHAR*, WCHAR*);

	//�ͷ�Shader
	void virtual ShutDown();

	//���Shader�����ļ��Ĵ�����Ϣ
	void virtual OutputShaderErrorMessage(ID3D10Blob*, WCHAR*);

public:

	//�任����������ṹ��
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
