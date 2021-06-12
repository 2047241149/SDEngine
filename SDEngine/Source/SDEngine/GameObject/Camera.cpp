#include"Camera.h"

Camera::Camera()
{
	mPosition = XMFLOAT3(0.f, 0.0f, -1.0f);
	mRight = XMFLOAT3(1.0f, 0.0f, 0.0f);
	mUp = XMFLOAT3(0.0f, 1.0f, 0.0f);
    mLook= XMFLOAT3(0.0f, 0.0f, 1.0f);
}

Camera::Camera(const Camera&other)
{

}

Camera::~Camera()
{

}

void Camera::SetPosition(float x, float y, float z)
{
	mPosition.x = x;
	mPosition.y = y;
	mPosition.z = z;
}

void Camera::SetPosition(const XMFLOAT3& v)
{
	mPosition = v;
}

XMFLOAT3 Camera::GetPosition()const
{
	return mPosition;
}

XMVECTOR Camera::GetPositionXM()const
{
	return XMLoadFloat3(&mPosition);
}

XMFLOAT3 Camera::GetUp()const
{
	return mUp;
}
XMVECTOR Camera::GetUpXM()const
{
	return XMLoadFloat3(&mUp);
}

XMFLOAT3 Camera::GetLook()const
{
	return mLook;
}

XMVECTOR Camera::GetLookXM()const
{
	return XMLoadFloat3(&mLook);
}


XMFLOAT3 Camera::GetRight()const
{

	return mRight;
}

XMVECTOR Camera::GetRightXM()const
{
	return XMLoadFloat3(&mRight);
}

XMMATRIX Camera::GetViewMatrix()const
{
	return mViewMatrix;
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

	XMStoreFloat3(&mPosition, pos);
	XMStoreFloat3(&mLook, Look);
	XMStoreFloat3(&mUp, Up);
	XMStoreFloat3(&mRight, Right);
}

void Camera::Walk(float d)
{
	//mPosition+=d*mLook
	//XMVectorReplicate���ص���XMVECTOR(d,d,d,d)
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&mLook);
	XMVECTOR p = XMLoadFloat3(&mPosition);
    
	//XMVectorMultiplyAdd(v1,v2,v3)=v1*v2+v3
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, l, p));
}

void Camera::Strafe(float d)
{
	//mPosition+=d*mRight
	//XMVectorReplicate���ص���XMVECTOR(d,d,d,d)
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&mRight);
	XMVECTOR p = XMLoadFloat3(&mPosition);

	//XMVectorMultiplyAdd(v1,v2,v3)=v1*v2+v3
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, r, p));
}

void Camera::UpDown(float d)
{

	//mPosition+=d*up
	//XMVectorReplicate���ص���XMVECTOR(d,d,d,d)
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f,0.0f);
	XMVECTOR p = XMLoadFloat3(&mPosition);

	//XMVectorMultiplyAdd(v1,v2,v3)=v1*v2+v3
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, up, p));
}

void Camera::Pitch(float angle)
{
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&mRight), angle);
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));
	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
} 

void Camera::RotateY(float angle)
{
	XMMATRIX R = XMMatrixRotationY(angle);
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));
	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mRight, XMVector3TransformNormal(XMLoadFloat3(&mRight), R));
}

void Camera::UpdateViewMatrix()
{
	XMVECTOR R = XMLoadFloat3(&mRight);
	XMVECTOR U = XMLoadFloat3(&mUp);
	XMVECTOR L = XMLoadFloat3(&mLook);
	XMVECTOR P = XMLoadFloat3(&mPosition);

	L = XMVector3Normalize(L);
	
	//U=look(X)right
	U = XMVector3Normalize(XMVector3Cross(L, R));

	//R=up(X)look
	R= XMVector3Normalize(XMVector3Cross(U, L));

	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));
	mViewMatrix.r[0] = XMVectorSet(mRight.x, mUp.x, mLook.x, 0);
	mViewMatrix.r[1] = XMVectorSet(mRight.y, mUp.y, mLook.y, 0);
	mViewMatrix.r[2] = XMVectorSet(mRight.z, mUp.z, mLook.z, 0);
	mViewMatrix.r[3] = XMVectorSet(x, y, z, 1.0f);
}


XMMATRIX Camera::GetProjectionMatrix()const
{
	return XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearPlane, mFarPlane);
}

float Camera::GetNearPlane()const
{
	return mNearPlane;
}

float Camera::GetFarPlane()const
{
	return mFarPlane;
}

void Camera::SetProjParams(float fovY, float aspect, float nearPlane,float farPlane)
{
	mFovY = fovY;
	mAspect = aspect;
	mNearPlane = nearPlane;
	mFarPlane = farPlane;
}

void Camera::SetUIOrthoParams(float screenWidth, float screenHeight)
{
	mScreenWidth = screenWidth;
	mScreenHeight = screenHeight;
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
	return XMMatrixOrthographicLH(mScreenWidth, mScreenHeight, mNearPlane, mFarPlane);
}

shared_ptr<Camera> Camera::Get()
{
	if (nullptr == m_pCamera)
	{
		m_pCamera = shared_ptr<Camera>(new Camera());
	}
	return m_pCamera;

}

shared_ptr<Camera> Camera::m_pCamera = nullptr;