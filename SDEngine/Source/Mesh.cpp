#include "Mesh.h"


Mesh::Mesh(string strFbxFileName):
	pureColor(XMFLOAT4(1.0f,1.0f,1.0f,1.0f)),
	bTransparent(false),
	bReflect(false),
	bCastShadow(true)
{

	LoadFBXModel(strFbxFileName);	
	InitBuffer();
}

Mesh::Mesh(const Mesh& other)
{

}


Mesh::~Mesh()
{

}

void Mesh::ShutDownSRV()
{
	map<string, ID3D11ShaderResourceView*>& mSRVMap = m_pFBXModel->mSRVMap;

	for (auto iter = mSRVMap.begin(); iter != mSRVMap.end(); ++iter)
	{
		ReleaseCOM(iter->second);
	}
}



bool Mesh::LoadTexture()
{
	vector<ModelData>& mModelList = m_pFBXModel->mModelList;
	for (UINT index = 0; index < mModelList.size(); ++index)
	{
		ModelData* mModelData = &mModelList[index];
		for (auto iter = mModelData->mMaterialMap.begin();
			iter != mModelData->mMaterialMap.end(); ++iter)
		{
			LoadSRVResource(iter->second.diffuseMapFileName, mModelData);
			LoadSRVResource(iter->second.bumpMapFileName, mModelData);
			LoadSRVResource(iter->second.specularMapFileName, mModelData);
			LoadSRVResource(iter->second.alphaMapFileName, mModelData);
		}
	}

	return true;
}

bool Mesh::LoadFBXModel(string strFbxFileName)
{
	if (GImportFBX->m_mapFBXModel.find(strFbxFileName) == GImportFBX->m_mapFBXModel.end())
	{
		m_pFBXModel = shared_ptr<FBXModelData>(new FBXModelData());
		GImportFBX->ImportFbxFile(strFbxFileName, m_pFBXModel->mModelList);
		LoadTexture();
		GImportFBX->m_mapFBXModel[strFbxFileName] = m_pFBXModel;
	}
	else
	{
		m_pFBXModel = GImportFBX->m_mapFBXModel[strFbxFileName];
	}

	if (nullptr != m_pFBXModel)
	{

	}
	return true;
}

void Mesh::LoadSRVResource(string strTexFileName, ModelData* model)
{
	if (strTexFileName == "")
	{
		return;
	}

	map<string, ID3D11ShaderResourceView*>& mSRVMap = m_pFBXModel->mSRVMap;
	//确保所有相对路径相同的纹理不重复加载
	if (mSRVMap.find(strTexFileName) == mSRVMap.end())
	{
		ID3D11ShaderResourceView* memSRV = nullptr;
		Resource::CreateShaderResourceViewFromFile(g_pDevice, Str2Wstr(strTexFileName).c_str(), &memSRV);
		if ((int)memSRV == 0xcccccccc)
		{
			memSRV = nullptr;
		}
		mSRVMap[strTexFileName] = memSRV;
	}
}


void Mesh::ShutDown()
{
	ShutDownSRV();
}


void Mesh::InitBuffer()
{
	if (nullptr == m_pFBXModel)
		return;

	vector<ModelData>& mModelList = m_pFBXModel->mModelList;

	for (UINT index = 0; index < mModelList.size(); ++index)
	{
		ModelData* mModelData = &mModelList[index];

		for (int i = 0; i < (int)mModelData->mMeshList.size(); ++i)
		{
			//第一,填充(顶点)缓存形容结构体和子资源数据结构体,并创建顶点缓存
			MeshData& mMesh = mModelData->mMeshList[i];

			D3D11_BUFFER_DESC vertexBufferDesc;
			vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			vertexBufferDesc.ByteWidth = sizeof(mMesh.mVertexData[0]) * mMesh.mVertexData.size();
			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertexBufferDesc.CPUAccessFlags = 0;
			vertexBufferDesc.MiscFlags = 0;
			vertexBufferDesc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA vertexData;
			vertexData.pSysMem = &mMesh.mVertexData[0];
			vertexData.SysMemPitch = 0;
			vertexData.SysMemSlicePitch = 0;
			g_pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &mMesh.mVertexBuffer);

			//第二,填充(索引)缓存形容结构体和子资源数据结构体,并创建索引缓存
			D3D11_BUFFER_DESC  indexBufferDesc;
			indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			indexBufferDesc.ByteWidth = sizeof(WORD) * mMesh.mIndexData.size();
			indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			indexBufferDesc.CPUAccessFlags = 0;
			indexBufferDesc.MiscFlags = 0;
			indexBufferDesc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA indexData;
			indexData.pSysMem = &mMesh.mIndexData[0];
			indexData.SysMemPitch = 0;
			indexData.SysMemSlicePitch = 0;
			g_pDevice->CreateBuffer(&indexBufferDesc, &indexData, &mMesh.mIndexBuffer);
		}

	}

}

void Mesh::ShutDownBuffer()
{
	vector<ModelData>& mModelList = m_pFBXModel->mModelList;
	for (UINT index = 0; index < mModelList.size(); ++index)
	{
		ModelData* mModelData = &mModelList[index];
		for (int i = 0; i < (int)mModelData->mMeshList.size(); ++i)
		{
			MeshData& mMesh = mModelData->mMeshList[i];
			ReleaseCOM(mMesh.mVertexBuffer);
			ReleaseCOM(mMesh.mIndexBuffer);
		}
	}
}