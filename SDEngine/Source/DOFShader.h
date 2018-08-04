#ifndef _DOF_SHADER_H
#define _DOF_SHADER_H

#include "CoreMini.h"
#include "CommonConstantBufferStruct.h"
#include "2DShader.h"
using namespace std;
using namespace DirectX;


class DOFShader:public Shader_2D
{




public:
	DOFShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	DOFShader(const DOFShader& other);
	virtual ~DOFShader();

public:
	bool SetShaderCB(ID3D11ShaderResourceView* screenRT, ID3D11ShaderResourceView* screenBlurRT, 
		ID3D11ShaderResourceView* depthRT,
		float dofStart,float dofRange, float farPlane, float nearPlane);
	bool SetShaderParams(ID3D11ShaderResourceView* screenRT, ID3D11ShaderResourceView* screenBlurRT,
		ID3D11ShaderResourceView* depthRT,
		float dofStart, float dofRange,float farPlane, float nearPlane);

private:
	void CreateConstantBuffer();

protected:
	virtual void  ShutDown() override;



private:
	struct CBDOF
	{
		float depthStart;
		float depthRange;
		float farPlane;
		float nearPlane;
	};

private:
	ID3D11Buffer* mCBDOF;
};
#endif 