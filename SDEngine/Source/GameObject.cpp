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

	//������ƬԪ
	g_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	vector<ModelData>& mModelList = m_pMesh->m_pFBXModel->mModelList;
	map<string, ID3D11ShaderResourceView*>& mSRVMap = m_pMesh->m_pFBXModel->mSRVMap;
	for (UINT index = 0; index < mModelList.size(); ++index)
	{
		
		ModelData* mModelData = &mModelList[index];

		vector<MeshData>& mMeshList = mModelData->mMeshList;

		for (UINT i = 0; i < mMeshList.size(); ++i)
		{
			
			//���ݲ���Id������Ӧ��VertexShader,PixelShader
			MeshData& mesh = mMeshList[i];

			MaterialTexFileName& material = mModelData->mMaterialMap[mesh.materialId];
			ID3D11ShaderResourceView* diffuseSRV = mSRVMap[material.diffuseMapFileName];
			ID3D11ShaderResourceView* bumpSRV = mSRVMap[material.bumpMapFileName];
			ID3D11ShaderResourceView* specSRV = mSRVMap[material.specularMapFileName];
			ID3D11ShaderResourceView* alphaSRV = mSRVMap[material.alphaMapFileName];

			//����ɫ����ģʽ
			if (eMaterialType == MaterialType::PURE_COLOR)
			{
				GShaderManager->SetPureColorShader(worldMatrix,surfaceColor);
			}
			//��������ͼ(������)
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
			//������ + ������ͼ
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
			//������ + ������ͼ
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
			//��������ͼ + ������ͼ + ������ͼ
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
			//�߿����ģʽ
			else if(eMaterialType == MaterialType::WIRE_FRAME)
			{
				GShaderManager->SetPureColorShader(worldMatrix, XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
			} 

			//������ȡ��Ȼ���
			else if (eMaterialType == MaterialType::DEPTH_BUFFER)
			{
				GShaderManager->SetDepthGetShader(worldMatrix);
			}

	
			//���ö��㻺��
			UINT stride = sizeof(mesh.mVertexData[0]); //ÿ������Ԫ�صĿ�ȴ�С������˵ÿ������Ԫ�صĴ�С
			UINT offset = 0;
			g_pDeviceContext->IASetVertexBuffers(0, 1, &mesh.mVertexBuffer, &stride, &offset);

			//������������
			g_pDeviceContext->IASetIndexBuffer(mesh.mIndexBuffer, DXGI_FORMAT_R16_UINT, 0); //WordΪ�����ֽ�																		  //�������˷�ʽ

			g_pDeviceContext->DrawIndexed(mesh.mIndexData.size(), 0, 0);
		}
	}

}

void GameObject::RenderMesh()
{

	//������ƬԪ
	g_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	vector<ModelData>& mModelList = m_pMesh->m_pFBXModel->mModelList;
	for (UINT index = 0; index < mModelList.size(); ++index)
	{

		ModelData* mModelData = &mModelList[index];

		vector<MeshData>& mMeshList = mModelData->mMeshList;

		for (UINT i = 0; i < mMeshList.size(); ++i)
		{

			//���ݲ���Id������Ӧ��VertexShader,PixelShader
			MeshData& mesh = mMeshList[i];

			//���ö��㻺��
			UINT stride = sizeof(mesh.mVertexData[0]); //ÿ������Ԫ�صĿ�ȴ�С������˵ÿ������Ԫ�صĴ�С
			UINT offset = 0;
			g_pDeviceContext->IASetVertexBuffers(0, 1, &mesh.mVertexBuffer, &stride, &offset);

			//������������
			g_pDeviceContext->IASetIndexBuffer(mesh.mIndexBuffer, DXGI_FORMAT_R16_UINT, 0); //WordΪ�����ֽ�																		  //�������˷�ʽ

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