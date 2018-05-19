#include"GameObject.h"

GameObject::GameObject(string fbxFileName)
{
	Initialize(fbxFileName);
}


GameObject::GameObject(const GameObject& other)
{

}

GameObject::~GameObject()
{
	Shutdown();
}

bool GameObject::Initialize(string fbxFileName)
{

	bool result;
	mFBXModel = shared_ptr<FBXModel>(new FBXModel());
	mTransform = shared_ptr<Transform>(new Transform());

	//��һ����ʼ���ⲿģ������,��������ڳ�ʼ������֮ǰ����Ϊ���㻺���������������ⲿģ������
	result = LoadFBXModel(fbxFileName);
	if (!result)
	{
		MessageBox(NULL, L"LoadFBXModel failure", L"ERROR", MB_OK);
		return false;
	}

	//�ڶ�,��ʼ������model�ĸ���mesh�Ķ��㻺�棬��������
	result = InitializeBuffer();
	if (!result)
	{
		MessageBox(NULL, L"Initialize Buffer failure", L"ERROR", MB_OK);
		return false;
	}

	return true;
}

void GameObject::Shutdown()
{
	ShutdownBuffer();
	ShutdownSRV();
}



bool GameObject::InitializeBuffer()
{
	ID3D11Device* d3dDevice = D3DClass::GetInstance()->GetDevice();


	vector<Model>& mModelList = mFBXModel->mModelList;
	for (UINT index = 0; index < mModelList.size(); ++index)
	{
		Model* mModelData = &mModelList[index];

		for (int i = 0; i < mModelData->mMeshList.size(); ++i)
		{
			//��һ,���(����)�������ݽṹ�������Դ���ݽṹ��,���������㻺��

			Mesh& mMesh = mModelData->mMeshList[i];

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
			HR(d3dDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &mMesh.mVertexBuffer));

			//�ڶ�,���(����)�������ݽṹ�������Դ���ݽṹ��,��������������
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
			HR(d3dDevice->CreateBuffer(&indexBufferDesc, &indexData, &mMesh.mIndexBuffer));
		}

	}

	return true;
}

void GameObject::ShutdownBuffer()
{
	vector<Model>& mModelList = mFBXModel->mModelList;
	for (UINT index = 0; index < mModelList.size(); ++index)
	{
		Model* mModelData = &mModelList[index];
		for (int i = 0; i < mModelData->mMeshList.size(); ++i)
		{
			Mesh& mMesh = mModelData->mMeshList[i];
			ReleaseCOM(mMesh.mVertexBuffer);
			ReleaseCOM(mMesh.mIndexBuffer);
		}
	}



}


bool GameObject::LoadTexture()
{
	vector<Model>& mModelList = mFBXModel->mModelList;
	for (UINT index = 0; index < mModelList.size(); ++index)
	{
		Model* mModelData = &mModelList[index];
		for (auto iter = mModelData->mMaterialMap.begin();
			iter != mModelData->mMaterialMap.end(); ++iter)
		{
			CheckSRVMap(iter->second.diffuseMapFileName, mModelData);
			CheckSRVMap(iter->second.bumpMapFileName, mModelData);
			CheckSRVMap(iter->second.specularMapFileName, mModelData);
			CheckSRVMap(iter->second.alphaMapFileName, mModelData);
		}
	}

	return true;
}

bool GameObject::LoadFBXModel(string fbxFileName)
{
	ImportFBX* singleInstace = ImportFBX::GetInstance();
	singleInstace->ImportFbxFile(fbxFileName,mFBXModel->mModelList);

	LoadTexture();

	return true;
}

void GameObject::CheckSRVMap(string texFileName, Model* model)
{
	ID3D11Device* d3dDevice = D3DClass::GetInstance()->GetDevice();

	if (texFileName == "")
	{
		return;
	}

	map<string, ID3D11ShaderResourceView*>& mSRVMap = mFBXModel->mSRVMap;
	//ȷ���������·����ͬ�������ظ�����
	if (mSRVMap.find(texFileName) == mSRVMap.end())
	{
		ID3D11ShaderResourceView* memSRV = nullptr;
		DXUTCreateShaderResourceViewFromFile(d3dDevice, Str2Wstr(texFileName).c_str(), &memSRV);
		if ((int)memSRV == 0xcccccccc)
		{
			memSRV = nullptr;
		}
		mSRVMap[texFileName] = memSRV;
	}
}


void GameObject::Render(MaterialType renderMode ,FXMVECTOR surfaceColor)
{
	ID3D11DeviceContext* d3dDeviceContext = D3DClass::GetInstance()->GetDeviceContext();
	ShaderManager* mShaderManager = ShaderManager::GetInstance();
	Camera* mainCamera = Camera::GetInstance();
	Light* dirLight = Light::GetInstnce();
	XMVECTOR lightDir = dirLight->GetLightDirection();
	XMVECTOR lightColor = dirLight->GetLightColor();

	XMMATRIX worldMatrix = this->GetWorldMatrix();

	XMVECTOR errorShaderColor = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);

	//������ƬԪ
	d3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	vector<Model>& mModelList = mFBXModel->mModelList;
	map<string, ID3D11ShaderResourceView*>& mSRVMap = mFBXModel->mSRVMap;
	for (UINT index = 0; index < mModelList.size(); ++index)
	{
		
		Model* mModelData = &mModelList[index];

		vector<Mesh>& mMeshList = mModelData->mMeshList;

		for (UINT i = 0; i < mMeshList.size(); ++i)
		{
			
			//���ݲ���Id������Ӧ��VertexShader,PixelShader
			Mesh& mesh = mMeshList[i];

			Material& material = mModelData->mMaterialMap[mesh.materialId];
			ID3D11ShaderResourceView* diffuseSRV = mSRVMap[material.diffuseMapFileName];
			ID3D11ShaderResourceView* bumpSRV = mSRVMap[material.bumpMapFileName];
			ID3D11ShaderResourceView* specSRV = mSRVMap[material.specularMapFileName];
			ID3D11ShaderResourceView* alphaSRV = mSRVMap[material.alphaMapFileName];

			//����ɫ����ģʽ
			if (renderMode == MaterialType::PURE_COLOR)
			{
				mShaderManager->SetPureColorShader(worldMatrix,surfaceColor);
			}
			//��������ͼ(������)
			else if (renderMode == MaterialType::DIFFUSE)
			{
				if (diffuseSRV == nullptr|| material.diffuseMapFileName =="")
				{
					mShaderManager->SetPureColorShader(worldMatrix,errorShaderColor);
				}
				else if (diffuseSRV)
				{
					mShaderManager->SetDiffuseShader(worldMatrix,diffuseSRV);
				}
			}
			//������ + ������ͼ
			else if (renderMode == MaterialType::DIFFUSE_NORMAL)
			{
				if (diffuseSRV && bumpSRV&&material.bumpMapFileName != "")
				{

					mShaderManager->SetDiffuseNormalShader(worldMatrix,diffuseSRV, bumpSRV);
				}
				else
				{
					mShaderManager->SetPureColorShader(worldMatrix,errorShaderColor);
				}
			}
			//������ + ������ͼ
			else if (renderMode == MaterialType::DIFFUSE_SPECULAR)
			{
				if (diffuseSRV && specSRV&&material.specularMapFileName != "")
				{
					XMVECTOR cameraPos = Camera::GetInstance()->GetPositionXM();
					mShaderManager->SetDiffuseSpecShader(worldMatrix,diffuseSRV, specSRV);
				}
				else
				{
					mShaderManager->SetPureColorShader(worldMatrix, errorShaderColor);
				}
			}
			//��������ͼ + ������ͼ + ������ͼ
			else if (renderMode == MaterialType::DIFFUSE_NORMAL_SPECULAR)
			{
				if (diffuseSRV && bumpSRV&&specSRV && material.bumpMapFileName != "" &&  material.specularMapFileName!="")
				{
					XMVECTOR cameraPos = Camera::GetInstance()->GetPositionXM();
					mShaderManager->SetDiffuseNormalSpecShader(worldMatrix,
						diffuseSRV, bumpSRV, specSRV);
				}
				else
				{
					mShaderManager->SetPureColorShader(worldMatrix,errorShaderColor);
				}
			}
			//�߿����ģʽ
			else if(renderMode == MaterialType::WIRE_FRAME)
			{
				mShaderManager->SetPureColorShader(worldMatrix, XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
			} 

			//������ȡ��Ȼ���
			else if (renderMode == MaterialType::DEPTH_BUFFER)
			{
				mShaderManager->SetDepthGetShader(worldMatrix);
			}

	
			//���ö��㻺��
			UINT stride = sizeof(mesh.mVertexData[0]); //ÿ������Ԫ�صĿ�ȴ�С������˵ÿ������Ԫ�صĴ�С
			UINT offset = 0;
			d3dDeviceContext->IASetVertexBuffers(0, 1, &mesh.mVertexBuffer, &stride, &offset);

			//������������
			d3dDeviceContext->IASetIndexBuffer(mesh.mIndexBuffer, DXGI_FORMAT_R16_UINT, 0); //WordΪ�����ֽ�																		  //�������˷�ʽ

			d3dDeviceContext->DrawIndexed(mesh.mIndexData.size(), 0, 0);
		}
	}

}

void GameObject::RenderMesh()
{
	ID3D11DeviceContext* d3dDeviceContext = D3DClass::GetInstance()->GetDeviceContext();
	//������ƬԪ
	d3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	vector<Model>& mModelList = mFBXModel->mModelList;
	for (UINT index = 0; index < mModelList.size(); ++index)
	{

		Model* mModelData = &mModelList[index];

		vector<Mesh>& mMeshList = mModelData->mMeshList;

		for (UINT i = 0; i < mMeshList.size(); ++i)
		{

			//���ݲ���Id������Ӧ��VertexShader,PixelShader
			Mesh& mesh = mMeshList[i];

			//���ö��㻺��
			UINT stride = sizeof(mesh.mVertexData[0]); //ÿ������Ԫ�صĿ�ȴ�С������˵ÿ������Ԫ�صĴ�С
			UINT offset = 0;
			d3dDeviceContext->IASetVertexBuffers(0, 1, &mesh.mVertexBuffer, &stride, &offset);

			//������������
			d3dDeviceContext->IASetIndexBuffer(mesh.mIndexBuffer, DXGI_FORMAT_R16_UINT, 0); //WordΪ�����ֽ�																		  //�������˷�ʽ

			d3dDeviceContext->DrawIndexed(mesh.mIndexData.size(), 0, 0);
		}
	}


}

void GameObject::ShutdownSRV()
{
	map<string, ID3D11ShaderResourceView*>& mSRVMap = mFBXModel->mSRVMap;

	for (auto iter = mSRVMap.begin(); iter != mSRVMap.end(); ++iter)
	{
		ReleaseCOM(iter->second);
	}
}

XMMATRIX GameObject::GetWorldMatrix()
{
	XMMATRIX scale = XMMatrixScaling(mTransform->localScale.x, mTransform->localScale.y, mTransform->localScale.z);

	XMMATRIX rotation = XMMatrixRotationX(mTransform->localRotation.x / 180.0f * XM_PI)
		* XMMatrixRotationY(mTransform->localRotation.y / 180.0f * XM_PI)
		* XMMatrixRotationZ(mTransform->localRotation.z / 180.0f * XM_PI);

	XMMATRIX translation = XMMatrixTranslation(mTransform->localPosition.x, mTransform->localPosition.y, mTransform->localPosition.z);

	XMMATRIX worldMatrix = scale * rotation * translation;

	return worldMatrix;
}