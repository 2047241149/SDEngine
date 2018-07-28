
#pragma once
#ifndef _MATH_TOOL_H
#define _MATH_TOOL_H
#include<DirectXMath.h>
using namespace DirectX;

class MathTool
{
public:
	//返回一个矩阵的逆矩阵的转置
	static XMMATRIX GetInvenseTranspose(CXMMATRIX ma);
};



#endif