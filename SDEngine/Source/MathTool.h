
#pragma once
#ifndef _MATH_TOOL_H
#define _MATH_TOOL_H
#include<DirectXMath.h>
using namespace DirectX;

namespace MathTool
{
	//返回一个矩阵的逆矩阵的转置
	XMMATRIX GetInvenseTranspose(CXMMATRIX ma);
	float Max(float a, float b);
};



#endif