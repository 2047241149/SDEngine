#include"Light.h"

Light::Light()
{

	mFov = XM_PIDIV4;
	mAspect = 1.0f;
	mNearPlane = 0.1f;
	mFarPlane = 1000.0f;
	mLightColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mAmbientLight = XMFLOAT3(0.3f, 0.3f, 0.3f);
}

Light::Light(const Light&other)
{

}

Light::~Light()
{

}


XMVECTOR Light::GetLightColor()
{
	return XMLoadFloat4(&mLightColor);
}



XMVECTOR Light::GetLightDirection()
{
	XMVECTOR lightDir = XMVectorSet(mLookAtPos.x-mPos.x, mLookAtPos.y - mPos.y, mLookAtPos.z - mPos.z,1.0f);
	return lightDir;

}

void Light::SetViewParams(FXMVECTOR pos, FXMVECTOR lookAtPos)
{
	XMStoreFloat3(&mPos, pos);
	XMStoreFloat3(&mLookAtPos, lookAtPos);
}


XMMATRIX Light::GetProjMatrix()
{
	return XMMatrixPerspectiveFovLH(mFov, mAspect, mNearPlane, mFarPlane);
	
}


XMMATRIX Light::GetViewMatrix()
{
	return XMMatrixLookAtLH(XMLoadFloat3(&mPos), XMLoadFloat3(&mLookAtPos), XMVectorSet(0.0f, 1.0f, 0.0f,0.0f));
}



void Light::SetLightColor(FXMVECTOR lightColor)
{
	XMStoreFloat4(&mLightColor, lightColor);

}

void Light::SetPos(XMFLOAT3 pos)
{
	mPos = pos;
}


void Light::SetTargetPos(XMFLOAT3 targetPos)
{
	mLookAtPos = targetPos;
}

void Light::SetProjPara(float fFov, float fAspect, float fNearPlane, float fFarPlane)
{
	mFov = fFov;
	mAspect = fAspect;
	mNearPlane = fNearPlane;
	mFarPlane = fFarPlane;
}

Light* Light::GetInstnce()
{
	if (light == nullptr)
	{
		light = shared_ptr<Light>(new Light());
	}
	return light.get();
}

void Light::SetAmbientLight(FXMVECTOR ambientLight)
{
	XMStoreFloat3(&this->mAmbientLight, ambientLight);
}

XMVECTOR Light::GetAmbientLight()
{
	return XMLoadFloat3(&mAmbientLight);
}

shared_ptr<Light> Light::light = nullptr;
