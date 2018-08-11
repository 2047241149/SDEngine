#pragma once
#ifndef _MESH_H
#define _MESH_H

#include "CoreMini.h"
#include "ImportFBX.h"
#include "LoadTexture/Resource.h"

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

class Mesh
{
public:
	Mesh(string strFbxFileName);
	Mesh(const Mesh& other);
	~Mesh();

private:

	void ShutDownSRV();
	void ShutDownBuffer();

	void ShutDown();
	
	bool LoadTexture();

	bool LoadFBXModel(string strFbxFileName);
	void LoadSRVResource(string strTexFileName, ModelData* model);

	void InitBuffer();

public:
	shared_ptr<FBXModelData> m_pFBXModel;

public:
	bool bTransparent;
	bool bReflect;
	MaterialType m_eMaterialType;

	//当处于纯色渲染模式下，会使用的属性
	XMFLOAT4 pureColor;  

};
#endif // !_MESH_H
