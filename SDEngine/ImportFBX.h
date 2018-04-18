#ifndef _IMPORT_FBX_H
#define _IMPORT_FBX_H

#include<d3d11_1.h>
#include<fbxsdk.h>
#include<iostream>
#include<DirectXMath.h>
#include<vector>
#include<memory>
#include"CommomVertexFormat.h"
#include<map>
#define CONVERT_SCENE_SCALE
using namespace std;
using namespace DirectX;

class ImportFBX
{
private:
	vector<MemFBXModel> mMemFBXModelData;
	FbxScene* mScene;
private:
	FbxManager* mFbxManager = nullptr;
	string fbxFileNamePre;
private:
	//读取一个场景的mesh类型数据
	void ReadSceneMeshData(FbxScene* scene);
	void ReadMeshNodeData(FbxNode* node);
	void ProcessMesh(FbxMesh* mesh, MemFBXModel* fbxModel);
	void ReadMeshMaterialIndex(FbxMesh* mesh, vector<Triangle>& triangleData);
	void ChangeModelData(Model* destModel,MemFBXModel* srcMemFBXModel);

	//三角化mesh
	void TriangulateRecursive(FbxNode* node);

private:
	//回收分配的数据
	void ClearMemFBXModelData();

private:
	//读取顶点的常见几何属性
	void ReadVertexPos(FbxMesh* mesh, int ctrlPointIndex,XMFLOAT3* pos);
	void ReadVertexColor(FbxMesh* mesh, int ctrlPointIndex,int vertexCount,XMFLOAT3* color);
	void ReadVertexNormal(FbxMesh* mesh, int ctrlPointIndex, int vertexCount, XMFLOAT3* normal);
	void ReadVertexTangent(FbxMesh* mesh, int ctrlPointIndex, int vertexCount, XMFLOAT3* normal);
	void ReadVertexUV(FbxMesh* mesh, int ctrlPointIndex, int uvIndex, XMFLOAT2* uv);

private:
	//读取材质属性
	void LoadMaterial(FbxMesh* mesh, map<int, Material>& materialMap);
	void LoadMaterialTexture(FbxSurfaceMaterial* surfaceMaterial, int materialIndex, map<int, Material>& materialMap);
	void ReadReletiveTextureFileName(FbxProperty* property, int materialIndex, map<int, Material>& materialMap);
	
private:
	void CalculateTriangleTangent(Triangle& triangle);
public:
	static shared_ptr<ImportFBX> instance;
public:
	ImportFBX();
	ImportFBX(const ImportFBX& other);
	~ImportFBX();

public:
	static ImportFBX* GetInstance();
	void ClearMem();
	void ImportFbxFile(string fbxFileName, vector<Model>& mFBXModel);

};

#endif 
