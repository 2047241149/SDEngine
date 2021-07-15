#pragma once
#include "CoreMinimal.h"

class Material;
class MeshComponent;
class TransformComponent;

class RendererContext
{
public:
	static void DrawMesh(MeshComponent* mesh, TransformComponent* transform);
};