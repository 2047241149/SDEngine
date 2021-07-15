#pragma once
#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H

#include "CoreMinimal.h"
#include "SDEngine/Texture/Texture.h"
#include "SDEngine/Import/ImportFBX.h"
#include "SDEngine/Light/Light.h"
#include "SDEngine/Shader/ShaderManager.h"
#include "Camera.h"
#include "SDEngine/Scene/MeshComponent.h"

using namespace std;
using namespace DirectX;

enum RenderMode
{
	UseMaterial,
	Simple,
};

class GameObject
{
public:
	shared_ptr<Transform> m_pTransform;
	shared_ptr<MeshComponent> m_pMesh;
	
public:
	GameObject();
	GameObject(const GameObject&);
	~GameObject();

public:
	bool Init();
	
	void Shutdown();
	
	void Render(RenderMode renderMode = RenderMode::UseMaterial);
	
	void RenderMesh();

	void DrawMesh();

	XMMATRIX GetWorldMatrix();

 	void SetMesh(shared_ptr<MeshComponent> pMesh);
};
#endif 
