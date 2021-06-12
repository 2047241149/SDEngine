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

	for (int index = 0; index < 6; index++)
	{
		XMVECTOR targetVec = XMLoadFloat3(&targets[index]);
		XMVECTOR upVec = XMLoadFloat3(&ups[index]);
		ViewMatrixs[index] = XMMatrixLookAtLH(pos, targetVec, upVec);
	}
}