#include"GameObject.h"

GameObject::GameObject()
{
	Init();
}


GameObject::GameObject(const GameObject& other)
{

}

GameObject::~GameObject()
{
	Shutdown();
}

bool GameObject::Init()
{

	bool result;
	m_pTransform = shared_ptr<Transform>(new Transform());

	return true;
}

void GameObject::Shutdown()
{

}




void GameObject::Render()
{
	MaterialType eMaterialType = m_pMesh->m_eMaterialType;
	XMVECTOR surfaceColor = XMLoadFloat4(&m_pMesh->pureColor);


	XMMATRIX worldMatrix = this->GetWorldMatrix();
	XMVECTOR errorShaderColor = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);

	//三角形片元
	g_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	vector<ModelData>& mModelList = m_pMesh->m_pFBXModel->mModelList;
	map<string, ID3D11ShaderResourceView*>& mSRVMap = m_pMesh->m_pFBXModel->mSRVMap;
	for (UINT index = 0; index < mModelList.size(); ++index)
	{
		
		ModelData* mModelData = &mModelList[index];

		vector<MeshData>& mMeshList = mModelData->mMeshList;

		for (UINT i = 0; i < mMeshList.size(); ++i)
		{
			
			//根据材质Id设置相应的VertexShader,PixelShader
			MeshData& mesh = mMeshList[i];

			MaterialTexFileName& material = mModelData->mMaterialMap[mesh.materialId];
			ID3D11ShaderResourceView* diffuseSRV = mSRVMap[material.diffuseMapFileName];
			ID3D11ShaderResourceView* bumpSRV = mSRVMap[material.bumpMapFileName];
			ID3D11ShaderResourceView* specSRV = mSRVMap[material.specularMapFileName];
			ID3D11ShaderResourceView* alphaSRV = mSRVMap[material.alphaMapFileName];

			//纯颜色绘制模式
			if (eMaterialType == MaterialType::PURE_COLOR)
			{
				GShaderManager->SetPureColorShader(worldMatrix,surfaceColor);
			}
			//漫反射贴图(仅仅有)
			else if (eMaterialType == MaterialType::DIFFUSE)
			{
				if (diffuseSRV == nullptr|| material.diffuseMapFileName =="")
				{
					GShaderManager->SetPureColorShader(worldMatrix,errorShaderColor);
				}
				else if (diffuseSRV)
				{
					GShaderManager->SetDiffuseShader(worldMatrix,diffuseSRV);
				}
			}
			//漫反射 + 法线贴图
			else if (eMaterialType == MaterialType::DIFFUSE_NORMAL)
			{
				if (diffuseSRV && bumpSRV&&material.bumpMapFileName != "")
				{

					GShaderManager->SetDiffuseNormalShader(worldMatrix,diffuseSRV, bumpSRV);
				}
				else
				{
					GShaderManager->SetPureColorShader(worldMatrix,errorShaderColor);
				}
			}
			//漫反射 + 镜面贴图
			else if (eMaterialType == MaterialType::DIFFUSE_SPECULAR)
			{
				if (diffuseSRV && specSRV&&material.specularMapFileName != "")
				{
				
					GShaderManager->SetDiffuseSpecShader(worldMatrix,diffuseSRV, specSRV);
				}
				else
				{
					GShaderManager->SetPureColorShader(worldMatrix, errorShaderColor);
				}
			}
			//漫反射贴图 + 法线贴图 + 镜面贴图
			else if (eMaterialType == MaterialType::DIFFUSE_NORMAL_SPECULAR)
			{
				if (diffuseSRV && bumpSRV&&specSRV && material.bumpMapFileName != "" &&  material.specularMapFileName!="")
				{
					GShaderManager->SetDiffuseNormalSpecShader(worldMatrix,
						diffuseSRV, bumpSRV, specSRV);
				}
				else
				{
					GShaderManager->SetPureColorShader(worldMatrix,errorShaderColor);
				}
			}
			//线框绘制模式
			else if(eMaterialType == MaterialType::WIRE_FRAME)
			{
				GShaderManager->SetPureColorShader(worldMatrix, XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
			} 

			//仅仅获取深度缓存
			else if (eMaterialType == MaterialType::DEPTH_BUFFER)
			{
				GShaderManager->SetDepthGetShader(worldMatrix);
			}

	
			//设置顶点缓存
			UINT stride = sizeof(mesh.mVertexData[0]); //每个顶点元素的跨度大小，或者说每个顶点元素的大小
			UINT offset = 0;
			g_pDeviceContext->IASetVertexBuffers(0, 1, &mesh.mVertexBuffer, &stride, &offset);

			//设置索引缓存
			g_pDeviceContext->IASetIndexBuffer(mesh.mIndexBuffer, DXGI_FORMAT_R16_UINT, 0); //Word为两个字节																		  //设置拓扑方式

			g_pDeviceContext->DrawIndexed(mesh.mIndexData.size(), 0, 0);
		}
	}

}

void GameObject::RenderMesh()
{

	//三角形片元
	g_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	vector<ModelData>& mModelList = m_pMesh->m_pFBXModel->mModelList;
	for (UINT index = 0; index < mModelList.size(); ++index)
	{

		ModelData* mModelData = &mModelList[index];

		vector<MeshData>& mMeshList = mModelData->mMeshList;

		for (UINT i = 0; i < mMeshList.size(); ++i)
		{

			//根据材质Id设置相应的VertexShader,PixelShader
			MeshData& mesh = mMeshList[i];

			//设置顶点缓存
			UINT stride = sizeof(mesh.mVertexData[0]); //每个顶点元素的跨度大小，或者说每个顶点元素的大小
			UINT offset = 0;
			g_pDeviceContext->IASetVertexBuffers(0, 1, &mesh.mVertexBuffer, &stride, &offset);

			//设置索引缓存
			g_pDeviceContext->IASetIndexBuffer(mesh.mIndexBuffer, DXGI_FORMAT_R16_UINT, 0); //Word为两个字节																		  //设置拓扑方式

			g_pDeviceContext->DrawIndexed(mesh.mIndexData.size(), 0, 0);
		}
	}

}


XMMATRIX GameObject::GetWorldMatrix()
{
	XMMATRIX scale = XMMatrixScaling(m_pTransform->localScale.x, m_pTransform->localScale.y, m_pTransform->localScale.z);

	XMMATRIX rotation = XMMatrixRotationX(m_pTransform->localRotation.x / 180.0f * XM_PI)
		* XMMatrixRotationY(m_pTransform->localRotation.y / 180.0f * XM_PI)
		* XMMatrixRotationZ(m_pTransform->localRotation.z / 180.0f * XM_PI);

	XMMATRIX translation = XMMatrixTranslation(m_pTransform->localPosition.x, m_pTransform->localPosition.y, m_pTransform->localPosition.z);

	XMMATRIX worldMatrix = scale * rotation * translation;

	return worldMatrix;
}


void GameObject::SetMesh(shared_ptr<Mesh> pMesh)
{
	m_pMesh = pMesh;
}