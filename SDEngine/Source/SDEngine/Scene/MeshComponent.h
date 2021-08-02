#pragma once

#pragma once
#include "CoreMinimal.h"
#include "SDEngine/Import/ImportFBX.h"
#include "SDEngine/LoadTexture/Resource.h"

class Material;

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

class MeshComponent
{
public:
	MeshComponent() = default;
	MeshComponent(string strFbxFileName);
	MeshComponent(const MeshComponent& other) = default;
	~MeshComponent();

private:
	void ShutDownBuffer();
	void ShutDown();
	bool LoadFBXModel(string strFbxFileName);
	void InitBuffer();

public:
	void SetMesh(const string& meshFile);
	void SetTexture(string fileName, TextureType textureType);
	void SetAlbedoTexture(string fileName);
	void SetNormalTexture(string fileName);
	void SetSpecularTexture(string fileName);
	void SetRoughnessTexture(string fileName);
	void SetMetalTexture(string fileName);
	void SetMaterial(shared_ptr<Material> material);
	shared_ptr<Material> GetMaterial();
	shared_ptr<Material> GetMaterial() const;

public:
	shared_ptr<FBXModelData> m_pFBXModel;

public:
	bool bTransparent;
	bool bReflect;
	bool bCastShadow;
	MaterialType m_eMaterialType;

	XMFLOAT4 pureColor;
	float roughness;
	float metal;

private:
	shared_ptr<Material> material;
	string meshFileName;

	friend class SceneSerializer;
};
