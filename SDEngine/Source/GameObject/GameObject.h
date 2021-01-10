#pragma once
#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H

#include "../Texture/Texture.h"
#include "../Common/CoreMini.h"
#include <vector>
#include "../Common/CommomVertexFormat.h"
#include "../Common/CommonFunction.h"
#include "../Import/ImportFBX.h"
#include "../Light/Light.h"
#include "../Shader/ShaderManager.h"
#include "Camera.h"
#include "Mesh.h"
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
	//Initialize�Ǵ���Ԫ��,Render������Ԫ��,Shutdown��Release
	bool Init();
	void Shutdown();

	void Render();

	void RenderMesh();
	XMMATRIX GetWorldMatrix();

	void SetMesh(shared_ptr<Mesh> pMesh);
};
#endif 
