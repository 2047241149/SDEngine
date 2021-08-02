#pragma once
#include "CoreMinimal.h"

class Material;
class MeshComponent;
class TransformComponent;

class RendererContext
{
public:
	static void DrawMesh(MeshComponent* mesh, TransformComponent* transform);

public:
	static RendererContext* Get();
	void Init();

private:
	static shared_ptr<RendererContext> instance;
	
private:
	shared_ptr<Material> nullMaterial;
};