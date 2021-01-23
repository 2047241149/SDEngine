
#pragma once
#ifndef _CUBE_CAMERA_H
#define _CUBE_CAMERA_H
//这个类封装了相机的位置和旋转量
#include<Windows.h>
#include<DirectXMath.h>
#include<iostream>
#include<memory>
using namespace DirectX;
using std::shared_ptr;


class CubeCamera
{
private:
	XMMATRIX projMartrix;
	XMMATRIX ViewMatrixs[6];

public:
	void BuildCamera(XMFLOAT3 cameraPos);
	XMMATRIX GetProjMatrix();
	XMMATRIX GetViewMatrix(int index);
	CubeCamera(float fovY = XM_PIDIV2, float aspect = 1.0f, float nearPlane = 0.1f, float farPlane = 50.0f);
};

#endif 
