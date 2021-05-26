
#pragma once
#ifndef _FMATH_H
#define _FMATH_H
#include<DirectXMath.h>
using namespace DirectX;

struct FMath
{
	//返回一个矩阵的逆矩阵的转置
	static XMMATRIX GetInvenseTranspose(CXMMATRIX ma);
	static XMMATRIX GetInvense(CXMMATRIX ma);
	static float Max(float left, float right);
	static float lerp(float left, float right, float percent);
	static float Log2(float num);
};


#endif