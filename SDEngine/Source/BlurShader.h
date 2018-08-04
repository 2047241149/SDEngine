#ifndef _BLUR_SHADER_H
#define _BLUR_SHADER_H


#include "CoreMini.h"
#include "2DShader.h"
#include"Camera.h"
#include"Light.h"
#include "CommonConstantBufferStruct.h"
using namespace std;
using namespace DirectX;


class BlurShader:public Shader_2D
{

public:
	BlurShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	BlurShader(const BlurShader& other);
	virtual ~BlurShader();
};
#endif 