#pragma once
#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H

#include"Texture.h"
#include "CoreMini.h"
#include<vector>
#include"CommomVertexFormat.h"
#include "CommonFunction.h"
#include "ImportFBX.h"
#include "Light.h"
#include "ShaderManager.h"
#include"Camera.h"
#include "Mesh.h"
using namespace std;
using namespace DirectX;

enum MaterialType
{
	PURE_COLOR,
	DIFFUSE,
	DIFFUSE_NORMAL,
	DIFFUSE_SPECULAR,
	DIFFUSE_NORMAL_SPECULAR,
	WIRE_FRAME,
	DEPTH_BUFFER
};

class GameObject
{
public:
	shared_ptr<Transform> m_pTransform;

private:
	shared_ptr<Mesh> m_pMesh;


private:
	//加载各种缓存
	bool InitBuffer();

	//释放各种缓存
	void ShutdownBuffer();

public:
	GameObject();
	GameObject(const GameObject&);
	~GameObject();

public:
	//Initialize是创建元素,Render是设置元素,Shutdown是Release
	bool Init();
	void Shutdown();

	void Render(MaterialType renderMode = MaterialType::PURE_COLOR, FXMVECTOR surfaceColor = XMVectorSet(1.0f,1.0f,1.0f,1.0f));

	void RenderMesh();
	XMMATRIX GetWorldMatrix();

	void SetMesh(shared_ptr<Mesh> pMesh);
};
#endif 
