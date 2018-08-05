#include "Mesh.h"


Mesh::Mesh(string strFbxFileName):
	pureColor(XMFLOAT4(1.0f,1.0f,1.0f,1.0f)),
	bTransparent(false),
	bReflect(false)
{
	if (m_mapFBXModelData.end() == m_mapFBXModelData.find(strFbxFileName))
	{
		m_pFBXModel = m_mapFBXModelData[strFbxFileName];
	}
	else
	{
		LoadFBXModel(strFbxFileName);
		m_mapFBXModelData[strFbxFileName] = m_pFBXModel;
	}
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
	ImportFBX* singleInstace = ImportFBX::GetInstance();
	singleInstace->ImportFbxFile(strFbxFileName, m_pFBXModel->mModelList);

	LoadTexture();

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