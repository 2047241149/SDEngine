#include "MeshComponent.h"
#include "SDEngine/Shader/Material.h"

MeshComponent::MeshComponent(string strFbxFileName) :
	pureColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)),
	roughness(1.0f),
	metal(1.0f),
	bTransparent(false),
	bReflect(false),
	bCastShadow(true)
{

	LoadFBXModel(strFbxFileName);
	InitBuffer();
}

MeshComponent::MeshComponent(const MeshComponent& other)
{

}

MeshComponent::~MeshComponent()
{

}

bool MeshComponent::LoadFBXModel(string strFbxFileName)
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

void MeshComponent::ShutDown()
{
}

void MeshComponent::InitBuffer()
{
	if (nullptr == m_pFBXModel)
		return;

	vector<ModelData>& mModelList = m_pFBXModel->mModelList;

	for (UINT index = 0; index < mModelList.size(); ++index)
	{
		ModelData* mModelData = &mModelList[index];

		for (int i = 0; i < (int)mModelData->mMeshList.size(); ++i)
		{
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

void MeshComponent::ShutDownBuffer()
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

void MeshComponent::SetAlbedoTexture(string fileName)
{
	SetTexture(fileName, AlbedoMap);
}

void MeshComponent::SetNormalTexture(string fileName)
{
	SetTexture(fileName, NormalMap);
}

void MeshComponent::SetSpecularTexture(string fileName)
{
	SetTexture(fileName, SpecularMap);
}

void MeshComponent::SetRoughnessTexture(string fileName)
{
	SetTexture(fileName, RoughnessMap);
}

void MeshComponent::SetMetalTexture(string fileName)
{
	SetTexture(fileName, MetalMap);
}

void MeshComponent::SetTexture(string fileName, TextureType textureType)
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

void MeshComponent::SetMaterial(shared_ptr<Material> material)
{
	this->material = material;
}

shared_ptr<Material> MeshComponent::GetMaterial()
{
	return this->material;
}

shared_ptr<Material> MeshComponent::GetMaterial() const
{
	return this->material;
}