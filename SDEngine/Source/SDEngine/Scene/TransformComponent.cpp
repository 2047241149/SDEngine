#include "TransformComponent.h"
#include "Common/GraphicsConfig.h"


XMMATRIX TransformComponent::GetWorldMatrix_Intermediate()
{
	XMMATRIX scaleMatrix = XMMatrixScaling(scale.x * GAME_ENGINE_UNIT_SCALE,
		scale.y * GAME_ENGINE_UNIT_SCALE, scale.z * GAME_ENGINE_UNIT_SCALE);

	XMMATRIX rotationMatrix = XMMatrixRotationX(rotation.x / 180.0f * XM_PI)
		* XMMatrixRotationY(rotation.y / 180.0f * XM_PI)
		* XMMatrixRotationZ(rotation.z / 180.0f * XM_PI);

	XMMATRIX translationMatrix = XMMatrixTranslation(position.x * GAME_ENGINE_UNIT_SCALE,
		position.y * GAME_ENGINE_UNIT_SCALE, position.z *  GAME_ENGINE_UNIT_SCALE);

	XMMATRIX worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

	return worldMatrix;
}

XMMATRIX TransformComponent::GetWorldMatrix()
{
	return  GetWorldMatrix_Intermediate();
}