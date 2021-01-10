
#pragma once
#ifndef _FMATH_H
#define _FMATH_H
#include<DirectXMath.h>
using namespace DirectX;

struct FMath
{
	//����һ�������������ת��
	static XMMATRIX GetInvenseTranspose(CXMMATRIX ma);
	static float Max(float left, float right);
	static float lerp(float left, float right, float percent);
};


#endif