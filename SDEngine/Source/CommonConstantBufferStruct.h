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
	float pad1;
	XMFLOAT3 ambientLight;
	float pad2;
};


struct CBPointLight
{
	//lightColor第四位存储的是光源亮度
	XMFLOAT4 lightColor;  
	XMFLOAT3 lightPos;
	float radius;
	XMFLOAT4 attenuation;
};



#endif // !_COMMON_CONSTANT_BUFFER_ST
