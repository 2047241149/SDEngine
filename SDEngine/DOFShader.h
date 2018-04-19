#ifndef _DOF_SHADER_H
#define _DOF_SHADER_H

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


class DOFShader
{

private:
	struct CBDOF
	{
		float depthStart;
		float depthRange;
		float farPlane;
		float nearPlane;
	};

private:

	ID3D11VertexShader* md3dVertexShader;
	ID3D11PixelShader* md3dPixelShader;
	ID3D11InputLayout* md3dInputLayout;  
	ID3D11SamplerState *mSamplerLinearWrap;
	ID3D11SamplerState *mSamplerLinearClamp;
	ID3D11Buffer* mCBDOF;

public:
	bool virtual Initialize(WCHAR* vsFilenPath, WCHAR* psFilenPath);

	//初始化Shader,用于创建InputLayout,VertexShader,PixelShader,常量缓存
	bool virtual InitializeShader(WCHAR*, WCHAR*);

	//释放Shader
	void virtual ShutDown();

	//输出Shader编译文件的错误信息
	void virtual OutputShaderErrorMessage(ID3D10Blob*, WCHAR*);

public:
	DOFShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	DOFShader(const DOFShader& other);
	virtual ~DOFShader();

public:
	bool SetShaderCB(ID3D11ShaderResourceView* screenRT, ID3D11ShaderResourceView* screenBlurRT, 
		ID3D11ShaderResourceView* depthRT,
		float dofStart,float dofRange, float farPlane, float nearPlane);
	bool SetShaderState();
	bool SetShaderParams(ID3D11ShaderResourceView* screenRT, ID3D11ShaderResourceView* screenBlurRT,
		ID3D11ShaderResourceView* depthRT,
		float dofStart, float dofRange,float farPlane, float nearPlane);
};
#endif 