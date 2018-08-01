#ifndef _BLUR_SHADER_H
#define _BLUR_SHADER_H


#include "CoreMini.h"
#include"Shader.h"
#include"Camera.h"
#include"Light.h"
#include "CommonConstantBufferStruct.h"
using namespace std;
using namespace DirectX;


class BlurShader
{
private:

	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;
	ID3D11InputLayout* m_pInputLayout;
	ID3D11Buffer* m_pCBCommon;
	ID3D11SamplerState *m_pWrapLinearSampler;
	ID3D11SamplerState *m_pClampPointSampler;

public:
	bool virtual Init(WCHAR* vsFilenPath, WCHAR* psFilenPath);

	//��ʼ��Shader,���ڴ���InputLayout,VertexShader,PixelShader,��������
	bool virtual InitShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);

	//�ͷ�Shader
	void virtual ShutDown();

public:
	BlurShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	BlurShader(const BlurShader& other);
	virtual ~BlurShader();

public:
	bool SetShaderCB(ID3D11ShaderResourceView* screenRT);
	bool SetShaderState();
	bool SetShaderParams(ID3D11ShaderResourceView* screenRT);
};
#endif 