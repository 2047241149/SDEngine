#pragma once
#include<d3d11_1.h>
#include<DirectXMath.h>
#include<iostream>
#include<string>
#include<vector>
#include<map>
using namespace std;
using namespace DirectX;


struct VertexPCNTT
{
	XMFLOAT3 pos;
	XMFLOAT3 color;
	XMFLOAT3 normal;
	XMFLOAT3 tangent;
	XMFLOAT2 uv;
};


struct Triangle
{
	VertexPCNTT vertexs[3];
	int MaterialId;
};


struct Material
{
	string diffuseMapFileName;
	string specularMapFileName;
	string alphaMapFileName;
	string bumpMapFileName;
};


struct Mesh
{
	vector<VertexPCNTT> mVertexData;
	vector<WORD> mIndexData;
	int materialId;
	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;
};


//临时的FBX模型数据
struct MemFBXModel
{
	vector<Triangle> mTriangleData;
	map<int, Material> mMaterialMap;
};

//FBX一个节点解析出多个不同材质的mesh，也就是一个Model
//所以一个存在多个FBXMesh阶段的FBX文件可以解析出n个Model
struct Model
{
	vector<Mesh> mMeshList;
	map<int, Material> mMaterialMap;
};


//保证了一个FBX加载的所有纹理文件都仅仅加载一次
struct FBXModel
{
	vector<Model> mModelList;
	map<string, ID3D11ShaderResourceView*> mSRVMap;
};



struct Transform
{
	XMFLOAT3 localPosition = XMFLOAT3(1.0f,1.0f,1.0f);
	XMFLOAT3 localScale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	XMFLOAT3 localRotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
};