#ifndef _SHADER_H
#define _SHADER_H

#include "CoreMini.h"
#include"Camera.h"
using namespace std;
using namespace DirectX;


class Shader_3D
{
private:

	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;
	ID3D11InputLayout* m_pInputLayout; 
	ID3D11Buffer* m_pCBCommon; 
	ID3D11SamplerState *m_pWrapLinearSampler; 
	ID3D11SamplerState *m_pClampPointSampler;


protected:
	bool virtual Init(WCHAR* vsFilenPath, WCHAR* psFilenPath);

	//初始化Shader,用于创建InputLayout,VertexShader,PixelShader,常量缓存
	bool virtual InitShader(WCHAR*, WCHAR*);
	//释放Shader
	void virtual ShutDown();

public:
	Shader_3D(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	Shader_3D(const Shader_3D& other);
	virtual ~Shader_3D();

public:
	bool SetShaderCB(CXMMATRIX worldMatrix);
	bool SetShaderState();
	bool SetShaderParams(CXMMATRIX worldMatrix);
};
#endif 