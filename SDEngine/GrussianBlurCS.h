#ifndef _GRUSSIAN_BLUR_CS_H
#define _GRUSSIAN_BLUR_CS_H

#include<Windows.h>
#include"Macro.h"
#include<d3d11_1.h>
#include<d3dcompiler.h>
#include<DirectXMath.h>
#include<fstream>
#include"D3DClass.h"
#include "Log.h"

using namespace std;
using namespace DirectX;

class RWRenderTexture;

class GrussianBlurCS
{

public:
	GrussianBlurCS(WCHAR* csFilenPath);
	GrussianBlurCS(const GrussianBlurCS& other);
	virtual ~GrussianBlurCS();

public:
	void Run(int nScreenWidth, int nScreenHeight,
		RWRenderTexture* pRWRenderTextureH, RWRenderTexture* pRWRenderTextureV, ID3D11ShaderResourceView* pSceneRT);

protected:
	bool virtual Init(WCHAR* csFilenPath);

	bool virtual InitShader(WCHAR* csFilenPath);

	// Õ∑≈Shader
	void virtual ShutDown();

private:
	ID3D11ComputeShader* m_pCSGrussianBlurHorizontal;
	ID3D11ComputeShader* m_pCSGrussianBlurVertical;

};
#endif 