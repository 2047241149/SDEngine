#pragma once
#include "CameraObject.h"

struct EditorCameraHelper
{
	//left right mov
	static void Strafe(CameraObject& camera, float d);

	//forward back Move
	static void Walk(CameraObject& camera, float d);

	//up down move
	static void UpDown(CameraObject& camera, float d);

	//rotate right vec
	static void Pitch(CameraObject& camera, float angle);

	//row y in wolrdspace (-90.0f<=rotateAngle<=90.0f)
	static void RotateY(CameraObject& camera, float angle);
};