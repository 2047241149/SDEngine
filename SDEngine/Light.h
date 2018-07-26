#pragma once
#ifndef _LIGHT_H
#define  _LIGHT_H

#include<Windows.h>
#include<DirectXMath.h>
#include<iostream>
#include<memory>
using namespace DirectX;
using std::shared_ptr;

//跟CameraClass的用法差不多
class Light
{
private:
	XMFLOAT4 mLightColor;
	XMFLOAT3 mPos;
	XMFLOAT3 mLookAtPos;
	XMFLOAT3 mAmbientLight;
	float mFov;
	float mAspect;
	float mNearPlane;
	float mFarPlane;

public:
	Light();
	Light(const Light&other);
	~Light();

	static Light* GetInstnce();
public:
	static shared_ptr<Light> light;


public:
	void SetLightColor(FXMVECTOR lightColor);
	void SetPos(XMFLOAT3 pos);
	void SetTargetPos(XMFLOAT3 targetPos);
	void SetProjPara(float fFov, float fAspect, float fNearPlane, float fFarPlane);
	void SetViewParams(FXMVECTOR pos, FXMVECTOR lookAtPos);
	void SetAmbientLight(FXMVECTOR ambientLight);

	XMVECTOR GetLightColor();
	XMVECTOR GetLightDirection();
	XMVECTOR GetAmbientLight();
	XMMATRIX GetProjMatrix();
	XMMATRIX GetViewMatrix();

};
#endif // !_LIGHT_CLASS
