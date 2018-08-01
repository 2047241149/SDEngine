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

	//第一，初始化外部模型数据,这个发生在初始化缓存之前，因为顶点缓存索引缓存来自外部模型数据
	result = LoadFBXModel(fbxFileName);
	if (!result)
	{
		MessageBox(NULL, L"LoadFBXModel failure", L"ERROR", MB_OK);
		return false;
	}

	//第二,初始化构成model的各个mesh的顶点缓存，索引缓存
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

	vector<Model>& mModelList = mFBXModel->mModelList;
	for (UINT index = 0; index < mModelList.size(); ++index)
	{
		Model* mModelData = &mModelList[index];

		for (int i = 0; i < (int)mModelData->mMeshList.size(); ++i)
		{
			//第一,填充(顶点)缓存形容结构体和子资源数据结构体,并创建顶点缓存

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
			HR(g_pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &mMesh.mVertexBuffer));

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
			HR(g_pDevice->CreateBuffer(&indexBufferDesc, &indexData, &mMesh.mIndexBuffer));
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
		for (int i = 0; i < (int)mModelData->mMeshList.size(); ++i)
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

	if (texFileName == "")
	{
		return;
	}

	map<string, ID3D11ShaderResourceView*>& mSRVMap = mFBXModel->mSRVMap;
	//确保所有相对路径相同的纹理不重复加载
	if (mSRVMap.find(texFileName) == mSRVMap.end())
	{
		ID3D11ShaderResourceView* memSRV = nullptr;
		DXUTCreateShaderResourceViewFromFile(g_pDevice, Str2Wstr(texFileName).c_str(), &memSRV);
		if ((int)memSRV == 0xcccccccc)
		{
			memSRV = nullptr;
		}
		mSRVMap[texFileName] = memSRV;
	}
}


void GameObject::Render(MaterialType renderMode ,FXMVECTOR surfaceColor)
{


	XMMATRIX worldMatrix = this->GetWorldMatrix();

	XMVECTOR errorShaderColor = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);

	//三角形片元
	g_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	vector<Model>& mModelList = mFBXModel->mModelList;
	map<string, ID3D11ShaderResourceView*>& mSRVMap = mFBXModel->mSRVMap;
	for (UINT index = 0; index < mModelList.size(); ++index)
	{
		
		Model* mModelData = &mModelList[index];

		vector<Mesh>& mMeshList = mModelData->mMeshList;

		for (UINT i = 0; i < mMeshList.size(); ++i)
		{
			
			//根据材质Id设置相应的VertexShader,PixelShader
			Mesh& mesh = mMeshList[i];

			Material& material = mModelData->mMaterialMap[mesh.materialId];
			ID3D11ShaderResourceView* diffuseSRV = mSRVMap[material.diffuseMapFileName];
			ID3D11ShaderResourceView* bumpSRV = mSRVMap[material.bumpMapFileName];
			ID3D11ShaderResourceView* specSRV = mSRVMap[material.specularMapFileName];
			ID3D11ShaderResourceView* alphaSRV = mSRVMap[material.alphaMapFileName];

			//纯颜色绘制模式
			if (renderMode == MaterialType::PURE_COLOR)
			{
				GShaderManager->SetPureColorShader(worldMatrix,surfaceColor);
			}
			//漫反射贴图(仅仅有)
			else if (renderMode == MaterialType::DIFFUSE)
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
			else if (renderMode == MaterialType::DIFFUSE_NORMAL)
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
			else if (renderMode == MaterialType::DIFFUSE_SPECULAR)
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
			else if (renderMode == MaterialType::DIFFUSE_NORMAL_SPECULAR)
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
			else if(renderMode == MaterialType::WIRE_FRAME)
			{
				GShaderManager->SetPureColorShader(worldMatrix, XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
			} 

			//仅仅获取深度缓存
			else if (renderMode == MaterialType::DEPTH_BUFFER)
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

	vector<Model>& mModelList = mFBXModel->mModelList;
	for (UINT index = 0; index < mModelList.size(); ++index)
	{

		Model* mModelData = &mModelList[index];

		vector<Mesh>& mMeshList = mModelData->mMeshList;

		for (UINT i = 0; i < mMeshList.size(); ++i)
		{

			//根据材质Id设置相应的VertexShader,PixelShader
			Mesh& mesh = mMeshList[i];

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