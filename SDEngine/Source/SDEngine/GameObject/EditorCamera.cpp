#include "SDEngine/FPS.h"
#include "SDEngine/Input.h"
#include "EditorCamera.h"


EditorCamera::EditorCamera(float inFovY, float inAspect, float inNearPlane, float inFarPlane):
	CameraObject()
{
	SetProjParams(inFovY, inAspect, inNearPlane, inFarPlane);
}


void EditorCamera::Tick(float deltaTime)
{
	int mouseXOffset, mouseYOffset;
	static float rotateY = 0.0f;
	int fps = GFPS->GetFPS();
	Input::GetMousePositionOffset(mouseXOffset, mouseYOffset);

	//Update Editor Camera Pos
	if (Input::IsMouseButtuonPressed(EMouse::Right) && fps >= 5 && fps <= 1000000)
	{
		if (Input::IsKeyDown(EKey::W))
		{
			Walk(deltaTime * cameraMoveSpeed);
		}
		else if (Input::IsKeyDown(EKey::S))
		{
			Walk(-deltaTime * cameraMoveSpeed);
		}

		if (Input::IsKeyDown(EKey::A))
		{
			Strafe(-deltaTime * cameraMoveSpeed);
		}
		else if (Input::IsKeyDown(EKey::D))
		{
			Strafe(deltaTime * cameraMoveSpeed);
		}

		if (Input::IsKeyDown(EKey::Q))
		{
			UpDown(-deltaTime * cameraMoveSpeed);
		}
		else if (Input::IsKeyDown(EKey::E))
		{
			UpDown(deltaTime * cameraMoveSpeed);
		}

		if (rotateY <= 90.0f && rotateY >= -90.0f)
		{
			Pitch((float)mouseYOffset * deltaTime * cameraRotateSpeed);
		}

		RotateY((float)-mouseXOffset * deltaTime* cameraRotateSpeed);
	}

	UpdateViewMatrix();
}

void EditorCamera::Walk(float d)
{
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&look);
	XMVECTOR p = XMLoadFloat3(&position);
	XMStoreFloat3(&position, XMVectorMultiplyAdd(s, l, p));
}

void EditorCamera::Strafe(float d)
{
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&right);
	XMVECTOR p = XMLoadFloat3(&position);
	XMStoreFloat3(&position, XMVectorMultiplyAdd(s, r, p));
}

void EditorCamera::UpDown(float d)
{
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR p = XMLoadFloat3(&position);
	XMStoreFloat3(&position, XMVectorMultiplyAdd(s, up, p));
}

void EditorCamera::Pitch(float angle)
{
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&right), angle);
	XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), R));
	XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), R));
}

void EditorCamera::RotateY(float angle)
{
	XMMATRIX R = XMMatrixRotationY(angle);
	XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), R));
	XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), R));
	XMStoreFloat3(&right, XMVector3TransformNormal(XMLoadFloat3(&right), R));
}