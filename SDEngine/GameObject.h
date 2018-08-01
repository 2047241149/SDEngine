#pragma once
#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H

#include"Texture.h"
#include<d3d11_1.h>
#include<DirectXMath.h>
#include"Macro.h"  //包含辅助的宏
#include<string>
#include<fstream>
#include<iostream>
#include <sstream>
#include<memory>
#include<vector>
#include "DirectxCore.h"
#include"CommomVertexFormat.h"
#include "CommonFunction.h"
#include "ImportFBX.h"
#include "Light.h"
#include "ShaderManager.h"
#include"Camera.h"
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

private:
	//从一个FBX文件解析出N个Model
	shared_ptr<FBXModel> mFBXModel;
public:
	shared_ptr<Transform> mTransform;

private:
	//加载各种缓存
	bool InitializeBuffer();

	//释放各种缓存
	void ShutdownBuffer();


	//释放各种纹理资源
	void ShutdownSRV();

	//加载纹理
	bool LoadTexture(); 

	bool LoadFBXModel(string FbxFileName);

	void CheckSRVMap(string texFileName,Model* model);

public:
	GameObject(string FbxFileName);
	GameObject(const GameObject&);
	~GameObject();

public:
	//Initialize是创建元素,Render是设置元素,Shutdown是Release
	bool Initialize(string FbxFileName);
	void Shutdown();

	void Render(MaterialType renderMode = MaterialType::PURE_COLOR, FXMVECTOR surfaceColor = XMVectorSet(1.0f,1.0f,1.0f,1.0f));

	void RenderMesh();
	XMMATRIX GetWorldMatrix();
};
#endif 
