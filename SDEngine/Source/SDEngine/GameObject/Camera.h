
#pragma once
#ifndef _CAMERA_H
#define _CAMERA_H

#include<Windows.h>
#include<DirectXMath.h>
#include<iostream>
#include<memory>
using namespace DirectX;
using std::shared_ptr;

class Camera
{
	//TODO: attri will become private, use getXXX 
public:
	//View Params
	XMFLOAT3 position;
	XMFLOAT3 right;
	XMFLOAT3 up;
	XMFLOAT3 look;

	//Proj Params
	float fovY;
	float aspect;
	float nearPlane;
	float farPlane;

	//TODO: remove screenWidth and screenHeight, it is in GGameWindow
	float screenWidth, screenHeight;

	XMMATRIX viewMatrix;
	XMMATRIX uiViewMatrix;
	XMMATRIX projMatrix;

public:
	Camera();
	Camera(const Camera& other);
	~Camera();

	static shared_ptr<Camera> Get();

private:
	static shared_ptr<Camera> single;

public:
	void SetPosition(float x, float y, float z);
	void SetPosition(const XMFLOAT3& v);
	void SetProjParams(float fovY = XM_PIDIV4, float aspect = 1.0f, float nearPlane = 0.1f, float farPlane = 500.0f);
	void SetUIOrthoParams(float screenWidth, float screenHeight);
	XMFLOAT3 GetPosition()const;
	XMVECTOR GetPositionXM()const;
	XMFLOAT3 GetUp()const;
	XMVECTOR GetUpXM()const;
	XMFLOAT3 GetLook()const;
	XMVECTOR GetLookXM()const;
	XMFLOAT3 GetRight()const;
	XMVECTOR GetRightXM()const;
	float GetNearPlane()const;
	float GetFarPlane()const;
	void LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp);
	XMMATRIX GetViewMatrix()const;
	XMMATRIX GetUIViewMatrix()const;
	XMMATRIX GetUIOrthoMatrix()const;
	XMMATRIX GetProjectionMatrix()const;
	void UpdateViewMatrix();

	//left right mov
	void Strafe(float d);

	//forward back Move
	void Walk(float d);

	//up down move
	void UpDown(float d);

	//rotate right vec
	void Pitch(float angle);

	//row y in wolrdspace (-90.0f<=rotateAngle<=90.0f)
	void RotateY(float angle);
};
#endif 

#define	GCamera (Camera::Get())
