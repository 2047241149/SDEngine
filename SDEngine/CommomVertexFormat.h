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


//��ʱ��FBXģ������
struct MemFBXModel
{
	vector<Triangle> mTriangleData;
	map<int, Material> mMaterialMap;
};

//FBXһ���ڵ�����������ͬ���ʵ�mesh��Ҳ����һ��Model
//����һ�����ڶ��FBXMesh�׶ε�FBX�ļ����Խ�����n��Model
struct Model
{
	vector<Mesh> mMeshList;
	map<int, Material> mMaterialMap;
};


//��֤��һ��FBX���ص����������ļ�����������һ��
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