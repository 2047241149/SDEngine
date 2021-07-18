#include "EditorCamera.h"

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