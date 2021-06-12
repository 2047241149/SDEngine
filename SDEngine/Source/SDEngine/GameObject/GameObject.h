#pragma once
#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H

#include "CoreMinimal.h"
#include "SDEngine/Texture/Texture.h"
#include "Import/ImportFBX.h"
#include "Light/Light.h"
#include "Shader/ShaderManager.h"
#include "Camera.h"
#include "Mesh.h"

enum RenderMode
{
	Material,
	Simple,
};
using namespace std;
using namespace DirectX;

class GameObject
{
public:
	shared_ptr<Transform> m_pTransform;
	shared_ptr<Mesh> m_pMesh;

public:
	GameObject();
	GameObject(const GameObject&);
	~GameObject();

public:
	//Initialize是创建元素,Render是设置元素,Shutdown是Release
	bool Init();
	void Shutdown();

	void Render(RenderMode renderMode = RenderMode::Material);

	void RenderMesh();
	XMMATRIX GetWorldMatrix();

	void SetMesh(shared_ptr<Mesh> pMesh);
};
#endif 
