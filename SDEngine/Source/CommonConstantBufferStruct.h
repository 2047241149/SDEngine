#pragma once
#ifndef _COMMON_CONSTANT_BUFFER_ST
#define _COMMON_CONSTANT_BUFFER_ST


#include<d3d11_2.h>
#include<DirectXMath.h>
#include<iostream>
#include<string>
#include<vector>
#include<map>
using namespace std;
using namespace DirectX;

//
struct CBCommmon
{
	XMMATRIX mWorldMatrix;
	XMMATRIX mViewMatrix;
	XMMATRIX mProjMatrix;
	XMMATRIX mWorldInvTranposeMatirx;
	XMFLOAT3 cameraPos;
	float pad;
};


struct CBDirectionLight
{
	XMFLOAT4 lightColor;
	XMFLOAT3 lightDir;
	XMFLOAT3 ambientLight;
	XMFLOAT2 pad;
};


struct CBPointLight
{
	XMFLOAT4 lightColor;
	XMFLOAT3 lightPos;
	float radius;
	XMFLOAT3 attenuation;
	float pad;
};



#endif // !_COMMON_CONSTANT_BUFFER_ST
