#pragma once
#ifndef _MESH_H
#define _MESH_H

#include "../Common/CoreMini.h"
#include "../Import/ImportFBX.h"
#include "../LoadTexture/Resource.h"

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

enum TextureType
{
	AlbedoMap,
	NormalMap,
	SpecularMap,
	RoughnessMap,
	MetalMap,
};

class Mesh
{
public:
	Mesh(string strFbxFileName);
	Mesh(const Mesh& other);
	~Mesh();

private:
	void ShutDownBuffer();
	void ShutDown();
	bool LoadFBXModel(string strFbxFileName);
	void InitBuffer();

public:
	void SetTexture(string fileName, TextureType textureType);
	void SetAlbedoTexture(string fileName);
	void SetNormalTexture(string fileName);
	void SetSpecularTexture(string fileName);
	void SetRoughnessTexture(string fileName);
	void SetMetalTexture(string fileName);

public:
	shared_ptr<FBXModelData> m_pFBXModel;

public:
	bool bTransparent;
	bool bReflect;
	bool bCastShadow;
	MaterialType m_eMaterialType;

	//当处于纯色渲染模式下，会使用的属性
	XMFLOAT4 pureColor;  
	float roughness;
	float metal;
};
#endif // !_MESH_H
