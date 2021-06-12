#include "Mesh.h"


Mesh::Mesh(string strFbxFileName):
	pureColor(XMFLOAT4(1.0f,1.0f,1.0f,1.0f)),
	roughness(1.0f),
	metal(1.0f),
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

bool Mesh::LoadFBXModel(string strFbxFileName)
{
	if (GImportFBX->m_mapFBXModel.find(strFbxFileName) == GImportFBX->m_mapFBXModel.end())
	{
		m_pFBXModel = shared_ptr<FBXModelData>(new FBXModelData());
		GImportFBX->ImportFbxFile(strFbxFileName, m_pFBXModel->mModelList);
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

void Mesh::ShutDown()
{
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
			//��һ,���(����)�������ݽṹ�������Դ���ݽṹ��,���������㻺��
			MeshData& mMesh = mModelData->mMeshList[i];

			D3D11_BUFFER_DESC vertexBufferDesc;
			vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			vertexBufferDesc.ByteWidth = sizeof(mMesh.mVertexData[0]) * (UINT)mMesh.mVertexData.size();
			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertexBufferDesc.CPUAccessFlags = 0;
			vertexBufferDesc.MiscFlags = 0;
			vertexBufferDesc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA vertexData;
			vertexData.pSysMem = &mMesh.mVertexData[0];
			vertexData.SysMemPitch = 0;
			vertexData.SysMemSlicePitch = 0;
			g_pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &mMesh.mVertexBuffer);

			//�ڶ�,���(����)�������ݽṹ�������Դ���ݽṹ��,��������������
			D3D11_BUFFER_DESC  indexBufferDesc;
			indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			indexBufferDesc.ByteWidth = sizeof(WORD) * (UINT)mMesh.mIndexData.size();
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

void Mesh::SetAlbedoTexture(string fileName)
{
	SetTexture(fileName, AlbedoMap);
}

void Mesh::SetNormalTexture(string fileName)
{
	SetTexture(fileName, NormalMap);
}

void Mesh::SetSpecularTexture(string fileName)
{
	SetTexture(fileName, SpecularMap);
}

void Mesh::SetRoughnessTexture(string fileName)
{
	SetTexture(fileName, RoughnessMap);
}

void Mesh::SetMetalTexture(string fileName)
{
	SetTexture(fileName, MetalMap);
}

void Mesh::SetTexture(string fileName, TextureType textureType)
{
	if (nullptr == m_pFBXModel)
		return;

	vector<ModelData>& modelList = m_pFBXModel->mModelList;
	for (UINT index = 0; index < modelList.size(); ++index)
	{
		ModelData* modelData = &modelList[index];

		if (nullptr != modelData)
		{
			for (auto& it : modelData->mMaterialMap)
			{
				switch (textureType)
				{

				case AlbedoMap:
					it.second.diffuseMapFileName = fileName;
					break;
				case NormalMap:
					it.second.bumpMapFileName = fileName;
					break;
				case SpecularMap:
					it.second.specularMapFileName = fileName;
					break;
				case RoughnessMap:
					it.second.roughnessMapFileName = fileName;
					break;
				case MetalMap:
					it.second.metalMapFileName = fileName;
					break;
				default:
					break;
				}

				it.second.diffuseMapFileName = fileName;
			}
		}
	}
}