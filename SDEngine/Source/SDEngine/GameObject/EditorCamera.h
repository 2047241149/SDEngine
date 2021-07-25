#pragma once
#include "CameraObject.h"


class EditorCamera : public CameraObject
{
public:
	EditorCamera(float fovY = XM_PIDIV4, float aspect = 1.0f, float nearPlane = 0.1f, float farPlane = 500.0f);

public:
	float cameraMoveSpeed = 2.0f;
	float cameraRotateSpeed = 1.0f;

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

	void Tick(float deltaTime);

};