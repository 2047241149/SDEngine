
#pragma once
#ifndef _MATH_TOOL_H
#define _MATH_TOOL_H
#include<DirectXMath.h>
using namespace DirectX;

namespace MathTool
{
	//����һ�������������ת��
	XMMATRIX GetInvenseTranspose(CXMMATRIX ma);
	float Max(float a, float b);
};



#endif