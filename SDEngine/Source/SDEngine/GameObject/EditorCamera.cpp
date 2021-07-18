#include "EditorCamera.h"
#include "SDEngine/Scene/CameraComponent.h"
#include "SDEngine/Scene/TransformComponent.h"
#include "SDEngine/FPS.h"
#include "SDEngine/Input.h"

void EditorCameraHelper::Strafe(CameraObject& camera, float d)
{
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&camera.right);
	XMVECTOR p = XMLoadFloat3(&camera.position);
	XMStoreFloat3(&camera.position, XMVectorMultiplyAdd(s, r, p));
}

void EditorCameraHelper::Walk(CameraObject& camera, float d)
{
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&camera.look);
	XMVECTOR p = XMLoadFloat3(&camera.position);
	XMStoreFloat3(&camera.position, XMVectorMultiplyAdd(s, l, p));
}

void EditorCameraHelper::UpDown(CameraObject& camera, float d)
{
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR p = XMLoadFloat3(&camera.position);
	XMStoreFloat3(&camera.position, XMVectorMultiplyAdd(s, up, p));
}

void EditorCameraHelper::Pitch(CameraObject& camera, float angle)
{
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&camera.right), angle);
	XMStoreFloat3(&camera.look, XMVector3TransformNormal(XMLoadFloat3(&camera.look), R));
	XMStoreFloat3(&camera.up, XMVector3TransformNormal(XMLoadFloat3(&camera.up), R));
}

void EditorCameraHelper::RotateY(CameraObject& camera, float angle)
{
	XMMATRIX R = XMMatrixRotationY(angle);
	XMStoreFloat3(&camera.look, XMVector3TransformNormal(XMLoadFloat3(&camera.look), R));
	XMStoreFloat3(&camera.up, XMVector3TransformNormal(XMLoadFloat3(&camera.up), R));
	XMStoreFloat3(&camera.right, XMVector3TransformNormal(XMLoadFloat3(&camera.right), R));
}

void EditorCameraLogic::BeginPlay()
{
}

void EditorCameraLogic::Tick(float deltaTime)
{
	int mouseXOffset, mouseYOffset;
	static float rotateY = 0.0f;
	int fps = GFPS->GetFPS();
	Input::GetMousePositionOffset(mouseXOffset, mouseYOffset);

	//Update Editor Camera Pos
	auto& cameraCpt = actor.GetComponent<CameraComponent>();
	auto& transformCpt = actor.GetComponent<TransformComponent>();
	CameraObject& camera = cameraCpt.camera;

	if (Input::IsMouseButtuonPressed(EMouse::Right) && fps >= 5 && fps <= 1000000)
	{
		if (Input::IsKeyDown(EKey::W))
		{
			EditorCameraHelper::Walk(camera, deltaTime * cameraCpt.cameraMoveSpeed);
		}
		else if (Input::IsKeyDown(EKey::S))
		{
			EditorCameraHelper::Walk(camera, -deltaTime * cameraCpt.cameraMoveSpeed);
		}

		if (Input::IsKeyDown(EKey::A))
		{
			EditorCameraHelper::Strafe(camera, -deltaTime * cameraCpt.cameraMoveSpeed);
		}
		else if (Input::IsKeyDown(EKey::D))
		{
			EditorCameraHelper::Strafe(camera, deltaTime * cameraCpt.cameraMoveSpeed);
		}

		if (Input::IsKeyDown(EKey::Q))
		{
			EditorCameraHelper::UpDown(camera, -deltaTime * cameraCpt.cameraMoveSpeed);
		}
		else if (Input::IsKeyDown(EKey::E))
		{
			EditorCameraHelper::UpDown(camera, deltaTime * cameraCpt.cameraMoveSpeed);
		}

		if (rotateY <= 90.0f && rotateY >= -90.0f)
		{
			EditorCameraHelper::Pitch(camera, (float)mouseYOffset * deltaTime* cameraCpt.cameraRotateSpeed);
		}

		EditorCameraHelper::RotateY(camera, (float)-mouseXOffset * deltaTime* cameraCpt.cameraRotateSpeed);
	}

	transformCpt.position = camera.GetPosition();
	camera.UpdateViewMatrix();
}