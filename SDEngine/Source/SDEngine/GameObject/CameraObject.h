
#pragma once
#include<Windows.h>
#include<DirectXMath.h>
#include<iostream>
#include<memory>
using namespace DirectX;
using std::shared_ptr;

class CameraObject
{

public:
	CameraObject();
	CameraObject(const CameraObject& other) = default;
	~CameraObject();

public:
	void SetPosition(const XMFLOAT3& v);
	XMFLOAT3 GetPosition() const;

	void SetProjParams(float fovY = XM_PIDIV4, float aspect = 1.0f, float nearPlane = 0.1f, float farPlane = 500.0f);
	void LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp);

	XMFLOAT3 GetUp() const;
	XMFLOAT3 GetLook() const;
	XMFLOAT3 GetRight() const;
	float GetNearPlane() const;
	float GetFarPlane() const;
	XMMATRIX GetViewMatrix() const;
	XMMATRIX GetProjectionMatrix() const;
	void UpdateViewMatrix();

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

private:
	XMMATRIX viewMatrix;
	XMMATRIX projMatrix;
};