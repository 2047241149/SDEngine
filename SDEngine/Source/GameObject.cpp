#include"GameObject.h"
#include "TextureSamplerManager.h"
#include "GraphicsConfig.h"
#include "TextureManager.h"

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

	m_pTransform = shared_ptr<Transform>(new Transform());
	return true;
}

void GameObject::Shutdown()
{
}

void GameObject::Render()
{
	MaterialType eMaterialType = m_pMesh->m_eMaterialType;
	XMMATRIX worldMatrix = this->GetWorldMatrix();
	XMFLOAT4 errorShaderColor = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

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

			MaterialTexFileName& material = mModelData->mMaterialMap[mesh.materialId];
			ID3D11ShaderResourceView* albedoSRV = LoadTexture(material.diffuseMapFileName);
			ID3D11ShaderResourceView* bumpSRV = LoadTexture(material.bumpMapFileName);
			ID3D11ShaderResourceView* specSRV = LoadTexture(material.specularMapFileName);
			ID3D11ShaderResourceView* alphaSRV = LoadTexture(material.alphaMapFileName);
			ID3D11ShaderResourceView* roughnessSRV = LoadTexture(material.roughnessMapFileName);
			ID3D11ShaderResourceView* metalSRV = LoadTexture(material.metalMapFileName);
			roughnessSRV = (roughnessSRV == nullptr ? GWhiteTexture : roughnessSRV);
			metalSRV = (metalSRV == nullptr ? GWhiteTexture : metalSRV);

			//纯颜色绘制模式 TODO:代码重复,需要重构
			if (eMaterialType == MaterialType::PURE_COLOR)
			{
				GShaderManager->pureColorShader->SetMatrix("World", worldMatrix);
				GShaderManager->pureColorShader->SetMatrix("View", GCamera->GetViewMatrix());
				GShaderManager->pureColorShader->SetMatrix("Proj", GCamera->GetProjectionMatrix());
				GShaderManager->pureColorShader->SetMatrix("WorldInvTranspose", FMath::GetInvenseTranspose(worldMatrix));
				GShaderManager->pureColorShader->SetFloat4("surfaceColor", m_pMesh->pureColor);
				GShaderManager->pureColorShader->SetFloat("roughness", m_pMesh->roughness);
				GShaderManager->pureColorShader->SetFloat("metal", m_pMesh->metal);
				GShaderManager->pureColorShader->Apply();
			}
			//漫反射贴图(仅仅有)
			else if (eMaterialType == MaterialType::DIFFUSE)
			{
				if (albedoSRV == nullptr|| material.diffuseMapFileName =="")
				{
					GShaderManager->pureColorShader->SetMatrix("World", worldMatrix);
					GShaderManager->pureColorShader->SetMatrix("View", GCamera->GetViewMatrix());
					GShaderManager->pureColorShader->SetMatrix("Proj", GCamera->GetProjectionMatrix());
					GShaderManager->pureColorShader->SetMatrix("WorldInvTranspose", FMath::GetInvenseTranspose(worldMatrix));
					GShaderManager->pureColorShader->SetFloat4("surfaceColor", errorShaderColor);
					GShaderManager->pureColorShader->SetFloat("roughness", m_pMesh->roughness);
					GShaderManager->pureColorShader->SetFloat("metal", m_pMesh->metal);
					GShaderManager->pureColorShader->Apply();
				}
				else if (albedoSRV)
				{
					//考虑ViewMatrix,ProjMatrix,UIOrthoMatrix作为全局CB,以及全局SamplerState????
					GShaderManager->diffuseShader->SetMatrix("World", worldMatrix);
					GShaderManager->diffuseShader->SetMatrix("View", GCamera->GetViewMatrix());
					GShaderManager->diffuseShader->SetMatrix("Proj", GCamera->GetProjectionMatrix());
					GShaderManager->diffuseShader->SetMatrix("WorldInvTranspose", FMath::GetInvenseTranspose(worldMatrix));
					GShaderManager->diffuseShader->SetTexture("AlbedoTexture", albedoSRV);
					GShaderManager->diffuseShader->SetTexture("SpecularTexture", GWhiteTexture);
					GShaderManager->diffuseShader->SetTexture("RoughnessTexture", roughnessSRV);
					GShaderManager->diffuseShader->SetTexture("MetalTexture", metalSRV);
					GShaderManager->diffuseShader->SetTextureSampler("SampleWrapLinear", GTextureSamplerBilinearWrap);
					GShaderManager->diffuseShader->Apply();
				}
			}
			//漫反射 + 法线贴图
			else if (eMaterialType == MaterialType::DIFFUSE_NORMAL)
			{
				if (albedoSRV && bumpSRV&&material.bumpMapFileName != "")
				{
					GShaderManager->diffuseNormalShader->SetMatrix("World", worldMatrix);
					GShaderManager->diffuseNormalShader->SetMatrix("View", GCamera->GetViewMatrix());
					GShaderManager->diffuseNormalShader->SetMatrix("Proj", GCamera->GetProjectionMatrix());
					GShaderManager->diffuseNormalShader->SetMatrix("WorldInvTranspose", FMath::GetInvenseTranspose(worldMatrix));
					GShaderManager->diffuseNormalShader->SetTexture("AlbedoTexture", albedoSRV);
					GShaderManager->diffuseNormalShader->SetTexture("NormalTexture", bumpSRV);
					GShaderManager->diffuseNormalShader->SetTexture("SpecularTexture", GWhiteTexture);
					GShaderManager->diffuseNormalShader->SetTexture("RoughnessTexture", roughnessSRV);
					GShaderManager->diffuseNormalShader->SetTexture("MetalTexture", metalSRV);
					GShaderManager->diffuseNormalShader->SetTextureSampler("SampleWrapLinear", GTextureSamplerBilinearWrap);
					GShaderManager->diffuseNormalShader->Apply();
				}
				else
				{
					GShaderManager->pureColorShader->SetMatrix("World", worldMatrix);
					GShaderManager->pureColorShader->SetMatrix("View", GCamera->GetViewMatrix());
					GShaderManager->pureColorShader->SetMatrix("Proj", GCamera->GetProjectionMatrix());
					GShaderManager->pureColorShader->SetMatrix("WorldInvTranspose", FMath::GetInvenseTranspose(worldMatrix));
					GShaderManager->pureColorShader->SetFloat4("surfaceColor", errorShaderColor);
					GShaderManager->pureColorShader->SetFloat("roughness", m_pMesh->roughness);
					GShaderManager->pureColorShader->SetFloat("metal", m_pMesh->metal);
					GShaderManager->pureColorShader->Apply();
				}
			}
			//漫反射 + 镜面贴图
			else if (eMaterialType == MaterialType::DIFFUSE_SPECULAR)
			{
				if (albedoSRV && specSRV&&material.specularMapFileName != "")
				{
				
					GShaderManager->diffuseSpecShader->SetMatrix("World", worldMatrix);
					GShaderManager->diffuseSpecShader->SetMatrix("View", GCamera->GetViewMatrix());
					GShaderManager->diffuseSpecShader->SetMatrix("Proj", GCamera->GetProjectionMatrix());
					GShaderManager->diffuseSpecShader->SetMatrix("WorldInvTranspose", FMath::GetInvenseTranspose(worldMatrix));
					GShaderManager->diffuseSpecShader->SetTexture("AlbedoTexture", albedoSRV);
					GShaderManager->diffuseSpecShader->SetTexture("SpecularTexture", specSRV);
					GShaderManager->diffuseSpecShader->SetTexture("RoughnessTexture", roughnessSRV);
					GShaderManager->diffuseSpecShader->SetTexture("MetalTexture", metalSRV);
					GShaderManager->diffuseSpecShader->SetTextureSampler("SampleWrapLinear", GTextureSamplerBilinearWrap);
					GShaderManager->diffuseSpecShader->Apply();
				}
				else
				{
					GShaderManager->pureColorShader->SetMatrix("World", worldMatrix);
					GShaderManager->pureColorShader->SetMatrix("View", GCamera->GetViewMatrix());
					GShaderManager->pureColorShader->SetMatrix("Proj", GCamera->GetProjectionMatrix());
					GShaderManager->pureColorShader->SetMatrix("WorldInvTranspose", FMath::GetInvenseTranspose(worldMatrix));
					GShaderManager->pureColorShader->SetFloat4("surfaceColor", errorShaderColor);
					GShaderManager->pureColorShader->SetFloat("roughness", m_pMesh->roughness);
					GShaderManager->pureColorShader->SetFloat("metal", m_pMesh->metal);
					GShaderManager->pureColorShader->Apply();
				}
			}
			//漫反射贴图 + 法线贴图 + 镜面贴图
			else if (eMaterialType == MaterialType::DIFFUSE_NORMAL_SPECULAR)
			{
				if (albedoSRV && bumpSRV&&specSRV && material.bumpMapFileName != "" &&  material.specularMapFileName!="")
				{
					GShaderManager->diffuseNormalSpecShader->SetMatrix("World", worldMatrix);
					GShaderManager->diffuseNormalSpecShader->SetMatrix("View", GCamera->GetViewMatrix());
					GShaderManager->diffuseNormalSpecShader->SetMatrix("Proj", GCamera->GetProjectionMatrix());
					GShaderManager->diffuseNormalSpecShader->SetMatrix("WorldInvTranspose", FMath::GetInvenseTranspose(worldMatrix));
					GShaderManager->diffuseNormalSpecShader->SetTexture("AlbedoTexture", albedoSRV);
					GShaderManager->diffuseNormalSpecShader->SetTexture("NormalTexture", bumpSRV);
					GShaderManager->diffuseNormalSpecShader->SetTexture("SpecularTexture", specSRV);
					GShaderManager->diffuseNormalSpecShader->SetTexture("RoughnessTexture", roughnessSRV);
					GShaderManager->diffuseNormalSpecShader->SetTexture("MetalTexture", metalSRV);
					GShaderManager->diffuseNormalSpecShader->SetTextureSampler("SampleWrapLinear", 
						GTextureSamplerBilinearWrap);
					GShaderManager->diffuseNormalSpecShader->Apply();
				}
				else
				{
					GShaderManager->pureColorShader->SetMatrix("World", worldMatrix);
					GShaderManager->pureColorShader->SetMatrix("View", GCamera->GetViewMatrix());
					GShaderManager->pureColorShader->SetMatrix("Proj", GCamera->GetProjectionMatrix());
					GShaderManager->pureColorShader->SetMatrix("WorldInvTranspose", FMath::GetInvenseTranspose(worldMatrix));
					GShaderManager->pureColorShader->SetFloat4("surfaceColor", errorShaderColor);
					GShaderManager->pureColorShader->SetFloat("roughness", m_pMesh->roughness);
					GShaderManager->pureColorShader->SetFloat("metal", m_pMesh->metal);
					GShaderManager->pureColorShader->Apply();
				}
			}
			//线框绘制模式
			else if(eMaterialType == MaterialType::WIRE_FRAME)
			{
				GShaderManager->pureColorShader->SetMatrix("World", worldMatrix);
				GShaderManager->pureColorShader->SetMatrix("View", GCamera->GetViewMatrix());
				GShaderManager->pureColorShader->SetMatrix("Proj", GCamera->GetProjectionMatrix());
				GShaderManager->pureColorShader->SetMatrix("WorldInvTranspose", FMath::GetInvenseTranspose(worldMatrix));
				GShaderManager->pureColorShader->SetFloat4("surfaceColor", XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
				GShaderManager->pureColorShader->SetFloat("roughness", m_pMesh->roughness);
				GShaderManager->pureColorShader->SetFloat("metal", m_pMesh->metal);
				GShaderManager->pureColorShader->Apply();

			} 

			//仅仅获取深度缓存
			else if (eMaterialType == MaterialType::DEPTH_BUFFER)
			{
				GShaderManager->depthGetShader->SetMatrix("World", worldMatrix);
				GShaderManager->depthGetShader->SetMatrix("View", GCamera->GetViewMatrix());
				GShaderManager->depthGetShader->SetMatrix("Proj", GCamera->GetProjectionMatrix());
				GShaderManager->depthGetShader->Apply();
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
	XMMATRIX scale = XMMatrixScaling(m_pTransform->localScale.x * GAME_ENGINE_UNIT_SCALE, 
		m_pTransform->localScale.y * GAME_ENGINE_UNIT_SCALE, m_pTransform->localScale.z * GAME_ENGINE_UNIT_SCALE);

	XMMATRIX rotation = XMMatrixRotationX(m_pTransform->localRotation.x / 180.0f * XM_PI)
		* XMMatrixRotationY(m_pTransform->localRotation.y / 180.0f * XM_PI)
		* XMMatrixRotationZ(m_pTransform->localRotation.z / 180.0f * XM_PI);

	XMMATRIX translation = XMMatrixTranslation(m_pTransform->localPosition.x * GAME_ENGINE_UNIT_SCALE,
		m_pTransform->localPosition.y * GAME_ENGINE_UNIT_SCALE, m_pTransform->localPosition.z *  GAME_ENGINE_UNIT_SCALE);

	XMMATRIX worldMatrix = scale * rotation * translation;

	return worldMatrix;
}

void GameObject::SetMesh(shared_ptr<Mesh> pMesh)
{
	m_pMesh = pMesh;
}