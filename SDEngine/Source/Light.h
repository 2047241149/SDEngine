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
	XMFLOAT4 m_LightColor;
	XMFLOAT3 m_Position;

public:
	Light();
	Light(const Light&other);
	~Light();

public:
	void SetLightColor(XMFLOAT4 color);
	void SetLightPostion(XMFLOAT3 pos);
	XMFLOAT4 GetLightColor();
	XMFLOAT3 GetPosition();
};
#endif // !_LIGHT_CLASS
