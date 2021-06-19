#include"Camera.h"

Camera::Camera()
{
	position = XMFLOAT3(0.f, 0.0f, 5.0f);
	right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	up = XMFLOAT3(0.0f, 1.0f, 0.0f);
    look= XMFLOAT3(0.0f, 0.0f, -1.0f);
}

Camera::Camera(const Camera&other)
{

}

Camera::~Camera()
{

}

void Camera::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}

void Camera::SetPosition(const XMFLOAT3& v)
{
	position = v;
}

XMFLOAT3 Camera::GetPosition()const
{
	return position;
}

XMVECTOR Camera::GetPositionXM()const
{
	return XMLoadFloat3(&position);
}

XMFLOAT3 Camera::GetUp()const
{
	return up;
}
XMVECTOR Camera::GetUpXM()const
{
	return XMLoadFloat3(&up);
}

XMFLOAT3 Camera::GetLook()const
{
	return look;
}

XMVECTOR Camera::GetLookXM()const
{
	return XMLoadFloat3(&look);
}


XMFLOAT3 Camera::GetRight()const
{
	return right;
}

XMVECTOR Camera::GetRightXM()const
{
	return XMLoadFloat3(&right);
}

XMMATRIX Camera::GetViewMatrix()const
{
	return viewMatrix;
}

void Camera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
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

void Camera::Walk(float d)
{
	//mPosition+=d*mLook
	//XMVectorReplicate = XMVECTOR(d,d,d,d)
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&look);
	XMVECTOR p = XMLoadFloat3(&position);
    
	//XMVectorMultiplyAdd(v1,v2,v3)=v1*v2+v3
	XMStoreFloat3(&position, XMVectorMultiplyAdd(s, l, p));
}

void Camera::Strafe(float d)
{
	//mPosition+=d*mRight
	//XMVectorReplicate = XMVECTOR(d,d,d,d)
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&right);
	XMVECTOR p = XMLoadFloat3(&position);

	//XMVectorMultiplyAdd(v1,v2,v3)=v1*v2+v3
	XMStoreFloat3(&position, XMVectorMultiplyAdd(s, r, p));
}

void Camera::UpDown(float d)
{

	//mPosition+=d*up
	//XMVectorReplicate���ص���XMVECTOR(d,d,d,d)
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f,0.0f);
	XMVECTOR p = XMLoadFloat3(&position);

	//XMVectorMultiplyAdd(v1,v2,v3)=v1*v2+v3
	XMStoreFloat3(&position, XMVectorMultiplyAdd(s, up, p));
}

void Camera::Pitch(float angle)
{
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&right), angle);
	XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), R));
	XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), R));
} 

void Camera::RotateY(float angle)
{
	XMMATRIX R = XMMatrixRotationY(angle);
	XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), R));
	XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), R));
	XMStoreFloat3(&right, XMVector3TransformNormal(XMLoadFloat3(&right), R));
}

void Camera::UpdateViewMatrix()
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


XMMATRIX Camera::GetProjectionMatrix()const
{
	return XMMatrixPerspectiveFovLH(fovY, aspect, nearPlane, farPlane);
}

float Camera::GetNearPlane()const
{
	return nearPlane;
}

float Camera::GetFarPlane()const
{
	return farPlane;
}

void Camera::SetProjParams(float fovY, float aspect, float nearPlane,float farPlane)
{
	this->fovY = fovY;
	this->aspect = aspect;
	this->nearPlane = nearPlane;
	this->farPlane = farPlane;
}

void Camera::SetUIOrthoParams(float screenWidth, float screenHeight)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
}

XMMATRIX Camera::GetUIViewMatrix()const
{
	XMVECTOR Up, Postion, LookAt;
	Postion = XMVectorSet(0.0f, 0.0f, -10.0f, 0.0f);
	LookAt = XMVectorSet(0.0f, 0.0f, 100.0f, 0.0f);
	Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	return XMMatrixLookAtLH(Postion, LookAt, Up);
}

XMMATRIX Camera::GetUIOrthoMatrix()const
{
	return XMMatrixOrthographicLH(screenWidth, screenHeight, nearPlane, farPlane);
}

shared_ptr<Camera> Camera::Get()
{
	if (nullptr == single)
	{
		single = shared_ptr<Camera>(new Camera());
	}

	return single;
}

shared_ptr<Camera> Camera::single = nullptr;