﻿#ifndef _GRUSSIAN_BLUR_CS_H
#define _GRUSSIAN_BLUR_CS_H

#include "CoreMinimal.h"

class RWRenderTexture;

class GrussianBlurCS
{

public:
	GrussianBlurCS(WCHAR* csFilenPath);
	virtual ~GrussianBlurCS();

public:
	void Run(int nScreenWidth, int nScreenHeight,
		RWRenderTexture* pRWRenderTextureH, RWRenderTexture* pRWRenderTextureV, ID3D11ShaderResourceView* pSceneRT);

protected:
	bool virtual Init(WCHAR* csFilenPath);

	bool virtual InitShader(WCHAR* csFilenPath);

	//释放Shader
	void virtual ShutDown();

private:
	ID3D11ComputeShader* m_pCSGrussianBlurHorizontal;
	ID3D11ComputeShader* m_pCSGrussianBlurVertical;

};
#endif 