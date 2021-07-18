#pragma once
#include "SDEngine/GameObject/CameraObject.h"

class CameraComponent
{
public:
	CameraComponent(float fovY = XM_PIDIV4, float aspect = 1.0f, float nearPlane = 0.1f, float farPlane = 500.0f):
		cameraMoveSpeed(3.0f),
		cameraRotateSpeed(0.5f)
	{
		camera.SetProjParams(fovY, aspect, nearPlane, farPlane);
	}

	CameraComponent(const CameraComponent& other) = default;

public:
	CameraObject camera;
	bool bPrimary;
	float cameraMoveSpeed;
	float cameraRotateSpeed;
};