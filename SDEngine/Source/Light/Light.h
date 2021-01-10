#pragma once
#ifndef _LIGHT_H
#define  _LIGHT_H

#include<Windows.h>
#include<DirectXMath.h>
#include<iostream>
#include<memory>
using namespace DirectX;
using std::shared_ptr;


class Light
{
public:
	bool m_bCastShadow;

protected:
	XMFLOAT3 m_LightColor;
	XMFLOAT3 m_LightPosition;
	float m_fLightIntensity;

public:
	Light();
	Light(const Light&other);
	~Light();


public:
	void SetLightPostion(XMFLOAT3 pos);
	XMFLOAT3 GetLightColor();
	virtual XMFLOAT3 GetPosition();
	float GetLightIntensity();
	virtual void SetLightIntensity(float fLightIntensity);
	virtual void SetLightColor(XMFLOAT3 color);
};
#endif // !_LIGHT_CLASS
