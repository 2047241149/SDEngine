#pragma once
#ifndef _COMMON_CONSTANT_BUFFER_ST
#define _COMMON_CONSTANT_BUFFER_ST


#include<d3d11_1.h>
#include<DirectXMath.h>
#include<iostream>
#include<string>
#include<vector>
#include<map>
using namespace std;
using namespace DirectX;

//  ”√
struct CBCommmon
{
	XMMATRIX mWorldMatrix;
	XMMATRIX mViewMatrix;
	XMMATRIX mProjMatrix;
	XMMATRIX mWorldInvTranposeMatirx;
	XMFLOAT3 cameraPos;
	float pad;
	XMFLOAT4 dirLightColor;
	XMFLOAT3 dirLightDir;
	float pad1;
	XMFLOAT3 ambientLight;
	float pad2;
};

#endif // !_COMMON_CONSTANT_BUFFER_ST
