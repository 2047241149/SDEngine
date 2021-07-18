#include"CameraObject.h"

CameraObject::CameraObject()
{
	position = XMFLOAT3(0.f, 0.0f, 5.0f);
	right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	up = XMFLOAT3(0.0f, 1.0f, 0.0f);
    look= XMFLOAT3(0.0f, 0.0f, -1.0f);
	UpdateViewMatrix();
}

CameraObject::~CameraObject()
{
}

void CameraObject::SetPosition(const XMFLOAT3& v)
{
	position = v;
	UpdateViewMatrix();
}

XMFLOAT3 CameraObject::GetPosition()const
{
	return position;
}

XMFLOAT3 CameraObject::GetUp()const
{
	return up;
}

XMFLOAT3 CameraObject::GetLook()const
{
	return look;
}

XMFLOAT3 CameraObject::GetRight()const
{
	return right;
}

XMMATRIX CameraObject::GetViewMatrix()const
{
	return viewMatrix;
}

void CameraObject::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
{
	XMVECTOR Look = XMVectorSubtract(target, pos);
	XMVECTOR Up = worldUp;
	XMVECTOR Right = XMVector3Cross(Up, Look);
	Up = XMVector3Cross(Look, Right);

	XMVector3Normalize(Look);
	XMVector3Normalize(Up);
	XMVector3Normalize(Right);

	XMStoreFloat3(&position, pos);
	XMStoreFloat3(&look, Look);
	XMStoreFloat3(&up, Up);
	XMStoreFloat3(&right, Right);
}

void CameraObject::UpdateViewMatrix()
{
	XMVECTOR R = XMLoadFloat3(&right);
	XMVECTOR U = XMLoadFloat3(&up);
	XMVECTOR L = XMLoadFloat3(&look);
	XMVECTOR P = XMLoadFloat3(&position);

	L = XMVector3Normalize(L);
	
	//U=look(X)right
	U = XMVector3Normalize(XMVector3Cross(L, R));

	//R=up(X)look
	R= XMVector3Normalize(XMVector3Cross(U, L));

	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));
	viewMatrix.r[0] = XMVectorSet(right.x, up.x, look.x, 0);
	viewMatrix.r[1] = XMVectorSet(right.y, up.y, look.y, 0);
	viewMatrix.r[2] = XMVectorSet(right.z, up.z, look.z, 0);
	viewMatrix.r[3] = XMVectorSet(x, y, z, 1.0f);
}

XMMATRIX CameraObject::GetProjectionMatrix()const
{
	return XMMatrixPerspectiveFovLH(fovY, aspect, nearPlane, farPlane);
}

float CameraObject::GetNearPlane()const
{
	return nearPlane;
}

float CameraObject::GetFarPlane()const
{
	return farPlane;
}

void CameraObject::SetProjParams(float fovY, float aspect, float nearPlane,float farPlane)
{
	this->fovY = fovY;
	this->aspect = aspect;
	this->nearPlane = nearPlane;
	this->farPlane = farPlane;
}