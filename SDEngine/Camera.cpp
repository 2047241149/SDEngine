
#include"Camera.h"

Camera::Camera()
{
	//初始化第一人称相机的参数，刚开始相机是位于中间的
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

//相机位置
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



//获取相机的基础向量(Up,Look,Right)
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

//获取相机变换矩阵
XMMATRIX Camera::GetViewMatrix()const
{
	return mViewMatrix;
}

//通过相机在世界空间的位置，目标点，以及上向量来定义相机变换矩阵
void Camera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
{
	
	XMVECTOR Look = XMVectorSubtract(target, pos);
	XMVECTOR Up = worldUp;
	XMVECTOR Right = XMVector3Cross(Up, Look);
	Up = XMVector3Cross(Look, Right);

	//规格化三个先来
	XMVector3Normalize(Look);
	XMVector3Normalize(Up);
	XMVector3Normalize(Right);

	//将参数存入Camera类中
	XMStoreFloat3(&mPosition, pos);
	XMStoreFloat3(&mLook, Look);
	XMStoreFloat3(&mUp, Up);
	XMStoreFloat3(&mRight, Right);

}


//往相机坐标系Look的方向前进或者后退,然后此时Look,Up,Right都不改变,仅仅是位置在改变
void Camera::Walk(float d)
{
	//mPosition+=d*mLook
	//XMVectorReplicate返回的是XMVECTOR(d,d,d,d)
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&mLook);
	XMVECTOR p = XMLoadFloat3(&mPosition);
    
	//XMVectorMultiplyAdd(v1,v2,v3)=v1*v2+v3
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, l, p));
}

//往相机坐标系Right方向前进或者后退,然后此时Look,Up,Right都不改变,仅仅是位置在改变
void Camera::Strafe(float d)
{
	//mPosition+=d*mRight
	//XMVectorReplicate返回的是XMVECTOR(d,d,d,d)
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&mRight);
	XMVECTOR p = XMLoadFloat3(&mPosition);

	//XMVectorMultiplyAdd(v1,v2,v3)=v1*v2+v3
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, r, p));
}

//相机上下的移动是位置在Y值上的改变，而Look,Up,Right都不改变,仅仅是位置在改变
void Camera::UpDown(float d)
{

	//mPosition+=d*up
	//XMVectorReplicate返回的是XMVECTOR(d,d,d,d)
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f,0.0f);
	XMVECTOR p = XMLoadFloat3(&mPosition);

	//XMVectorMultiplyAdd(v1,v2,v3)=v1*v2+v3
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, up, p));
}


//Look和Up向量绕相机坐标系的right向量进行旋转，此时look和up向量改变，而right和poisiton不改变
void Camera::Pitch(float angle)
{
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&mRight), angle);
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));
	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
} 

//绕世界坐标系的Y轴进行旋转(而非相机坐标系的Up向量,这点容易受到惯性思维而误解，实在无法理解就用人的视角在现实世界是怎么旋转的粒子解释),look,up,right绕Y轴进行旋转，位置不变
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

	//正交规格化right,look,up
	
	//规格化look向量
	L = XMVector3Normalize(L);
	
	//U=look(X)right
	U = XMVector3Normalize(XMVector3Cross(L, R));

	//R=up(X)look
	R= XMVector3Normalize(XMVector3Cross(U, L));

	//求出相机变换矩阵某些参数里面的点积
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

//获取基础相机变换矩阵
XMMATRIX Camera::GetUIViewMatrix()const
{
	//上向量,位置向量,观察向量
	XMVECTOR Up, Postion, LookAt;

	//设置相机的位置
	Postion = XMVectorSet(0.0f, 0.0f, -10.0f, 0.0f);
	LookAt = XMVectorSet(0.0f, 0.0f, 100.0f, 0.0f);
	Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	return XMMatrixLookAtLH(Postion, LookAt, Up);

}

//获取UI正交投影矩阵
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