#ifndef _SSR_SHADER_H
#define _SSR_SHADER_H

#include<Windows.h>
#include"Macro.h"
#include<d3d11_1.h>
#include<d3dcompiler.h>
#include<DirectXMath.h>
#include<fstream>
#include"MathTool.h"
#include"DirectxCore.h"
#include "Log.h"
#include"Shader.h"
#include"Camera.h"
#include"Light.h"
#include "CommonConstantBufferStruct.h"
using namespace std;
using namespace DirectX;


class SSRShader
{
private:
	struct CBSSR
	{
		float farPlane;
		float nearPlane;
		XMFLOAT2  perspectiveValue;
	};

private:

	ID3D11VertexShader* md3dVertexShader;
	ID3D11PixelShader* md3dPixelShader;
	ID3D11InputLayout* md3dInputLayout; 
	ID3D11Buffer* mCBCommon; 
	ID3D11Buffer* mCBSSR;
	ID3D11SamplerState *mSamplerLinearWrap;
	ID3D11SamplerState *mSamplerLinearClamp;

public:
	bool virtual Initialize(WCHAR* vsFilenPath, WCHAR* psFilenPath);

	//初始化Shader,用于创建InputLayout,VertexShader,PixelShader,常量缓存
	bool virtual InitializeShader(WCHAR*, WCHAR*);

	//释放Shader
	void virtual ShutDown();


public:
	SSRShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	SSRShader(const SSRShader& other);
	virtual ~SSRShader();

public:
	bool SetShaderCB(const CXMMATRIX& worldMatrix, ID3D11ShaderResourceView* arraySRV[5],
		XMFLOAT2 perspectiveValue);
	bool SetShaderState();
	bool SetShaderParams(const CXMMATRIX& worldMatrix, ID3D11ShaderResourceView* arraySRV[5],
		XMFLOAT2 perspectiveValue);
};
#endif 