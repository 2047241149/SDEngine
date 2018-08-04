#ifndef _GRAPHICS_BLIT_SHADER_H
#define _GRAPHICS_BLIT_SHADER_H

#include "CoreMini.h"
#include"Camera.h"
#include"Light.h"
#include "CommonConstantBufferStruct.h"
using namespace std;
using namespace DirectX;
#include "2DShader.h"

class GraphcisBlitShader : public Shader_2D
{
public:
	GraphcisBlitShader(WCHAR* vsFilenPath, WCHAR* psFilenPath);
	GraphcisBlitShader(const GraphcisBlitShader& other);
	virtual ~GraphcisBlitShader();

public:
	bool SetShaderCB(ID3D11ShaderResourceView* screenRT);
	bool SetShaderParams(ID3D11ShaderResourceView* screenRT);
};
#endif 