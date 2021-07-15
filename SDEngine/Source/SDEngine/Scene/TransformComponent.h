#pragma once
#include "CoreMinimal.h"

class TestComponent
{
public:
	int a = 1;
	TestComponent() = default;
};

class TransformComponent
{
public:
	TransformComponent() = default;
	TransformComponent(XMFLOAT3 inPosition = XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3 inScale = XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3 inRotation = XMFLOAT3(0.0f, 0.0f, 0.0f)):
		position(inPosition),
		scale(inScale),
		rotation(inRotation)
	{

	}

	XMMATRIX GetWorldMatrix();

private:
	XMMATRIX GetWorldMatrix_Intermediate();

public:
	XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	XMFLOAT3 rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
};