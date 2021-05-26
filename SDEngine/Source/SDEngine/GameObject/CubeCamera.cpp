
#include"CubeCamera.h"

CubeCamera::CubeCamera(float fovY, float aspect, float nearPlane, float farPlane)
{
	projMartrix = XMMatrixPerspectiveFovLH(fovY, aspect, nearPlane, farPlane);
}

XMMATRIX CubeCamera::GetProjMatrix()
{
	return projMartrix;
}

XMMATRIX CubeCamera::GetViewMatrix(int index)
{
	return ViewMatrixs[index];
}

void CubeCamera::BuildCamera(XMFLOAT3 cameraPos)
{
	float x = cameraPos.x;
	float y = cameraPos.y;
	float z = cameraPos.z;

	XMFLOAT3 targets[6] =
	{
		XMFLOAT3(x + 1.0f,y,z),
		XMFLOAT3(x - 1.0f,y,z),
		XMFLOAT3(x ,y + 1.0f,z),
		XMFLOAT3(x ,y - 1.0f,z),
		XMFLOAT3(x,y,z + 1.0f),
		XMFLOAT3(x ,y,z - 1.0f),
	};

	XMFLOAT3 ups[6] =
	{
		XMFLOAT3(0.0f,1.0f,0.0f), //+X
		XMFLOAT3(0.0f,1.0f,0.0f), //-X
		XMFLOAT3(0.0f,0.0f,-1.0f), //+Y
		XMFLOAT3(0.0f,0.0f,1.0f), //-Y
		XMFLOAT3(0.0f,1.0f,0.0f), //+Z
		XMFLOAT3(0.0f,1.0f,0.0f), //-Z
	};

	XMVECTOR pos = XMLoadFloat3(&cameraPos);
	XMFLOAT3 memLook;
	XMFLOAT3 memUp;
	XMFLOAT3 memRight;

	for (int index = 0; index < 6; index++)
	{
		/*XMVECTOR target = XMLoadFloat3(&targets[index]);
		XMVECTOR Look = XMVectorSubtract(target, pos);
		XMVECTOR Up = XMLoadFloat3(&ups[index]);
		XMVECTOR Right = XMVector3Cross(Up, Look);
		Up = XMVector3Cross(Look, Right);

		//规格化三个先来
		Look = XMVector3Normalize(Look);
		Up = XMVector3Normalize(Up);
		Right = XMVector3Normalize(Right);

		XMStoreFloat3(&memLook, Look);
		XMStoreFloat3(&memUp, Up);
		XMStoreFloat3(&memRight, Right);

		XMVECTOR L = XMVector3Normalize(Look);
		XMVECTOR U = XMVector3Normalize(XMVector3Cross(L, Right));
		XMVECTOR R = XMVector3Normalize(XMVector3Cross(U, L));

		float x = -XMVectorGetX(XMVector3Dot(pos, R));
		float y = -XMVectorGetX(XMVector3Dot(pos, U));
		float z = -XMVectorGetX(XMVector3Dot(pos, L));*/
		XMVECTOR targetVec = XMLoadFloat3(&targets[index]);
		XMVECTOR upVec = XMLoadFloat3(&ups[index]);

		ViewMatrixs[index] = XMMatrixLookAtLH(pos, targetVec, upVec);
		/*XMMATRIX& viewMatrix = ViewMatrixs[index];
		viewMatrix.r[0] = XMVectorSet(memRight.x, memUp.x, memLook .x, 0);
		viewMatrix.r[1] = XMVectorSet(memRight.y, memUp.y, memLook.y, 0);
		viewMatrix.r[2] = XMVectorSet(memRight.z, memUp.z, memLook.z, 0);
		viewMatrix.r[3] = XMVectorSet(x, y, z, 1.0f);*/
	}
}