﻿#include"ImportFBX.h"


ImportFBX::ImportFBX()
{
	if (!mFbxManager)
	{
		mFbxManager = FbxManager::Create();
	}

}
ImportFBX::ImportFBX(const ImportFBX& other)
{

}
ImportFBX::~ImportFBX()
{
	this->ClearMem();
}

void ImportFBX::ClearMem()
{

	if (mFbxManager)
	{
		mFbxManager->Destroy();
		mFbxManager = nullptr;
	}
}


ImportFBX* ImportFBX::Get()
{
	if (m_pImportFBX== nullptr)
	{
		m_pImportFBX = shared_ptr<ImportFBX>(new ImportFBX());
	}
	return m_pImportFBX.get();
}

void ImportFBX::ImportFbxFile(string fbxFileName, vector<ModelData>& mFBXModel)
{
	ClearMemFBXModelData();

	string fileSpaceStr = "\\";
	UINT pos = (UINT)fbxFileName.rfind(fileSpaceStr);
	fbxFileNamePre = fbxFileName.substr(0, pos) + fileSpaceStr;

	FbxIOSettings * ios = FbxIOSettings::Create(mFbxManager, IOSROOT);
	mFbxManager->SetIOSettings(ios);

	FbxImporter* fbxImport = FbxImporter::Create(mFbxManager, "");
	if (!fbxImport->Initialize(fbxFileName.c_str(), -1, mFbxManager->GetIOSettings()))
	{
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", fbxImport->GetStatus().GetErrorString());
		exit(-1);
	}

	 mScene = FbxScene::Create(mFbxManager, "scene");
	

	fbxImport->Import(mScene);
	
	FbxAxisSystem directxAxisSys = FbxAxisSystem::DirectX;
	FbxAxisSystem sceneAxisSystem = mScene->GetGlobalSettings().GetAxisSystem();
	if (sceneAxisSystem != directxAxisSys)
	{
		FbxAxisSystem::DirectX.ConvertScene(mScene);
	}

	/*#ifdef CONVERT_SCENE_SCALE
	FbxSystemUnit sceneSysUnit = mScene->GetGlobalSettings().GetSystemUnit();
	if (sceneSysUnit.GetScaleFactor() != 1.0f)
	{
		FbxSystemUnit::cm.ConvertScene(mScene);
	}
	#endif*/

	TriangulateRecursive(mScene->GetRootNode());

	ReadSceneMeshData(mScene);

	mFBXModel.resize(mMemFBXModelData.size());

	for (UINT i = 0; i < mFBXModel.size(); ++i)
	{
		ChangeModelData(&mFBXModel[i],&mMemFBXModelData[i]);
	}
	

	fbxImport->Destroy();

}

void ImportFBX::ReadSceneMeshData(FbxScene* scene)
{
	FbxNode* rootNode = scene->GetRootNode();
	if (rootNode)
	{
		for (int i = 0; i < rootNode->GetChildCount(); i++)
		{
			ReadMeshNodeData(rootNode->GetChild(i));
		}
	}
}

void ImportFBX::ReadMeshNodeData(FbxNode* node)
{
	FbxNodeAttribute::EType nodeType = node->GetNodeAttribute()->GetAttributeType();
	if (nodeType == FbxNodeAttribute::EType::eMesh)
	{
		FbxMesh* mesh = node->GetMesh();
		MemFBXModelData fbxModel;
		mMemFBXModelData.push_back(fbxModel);
		ProcessMesh(mesh, &mMemFBXModelData[mMemFBXModelData.size()-1]);
		LoadMaterial(mesh, mMemFBXModelData[mMemFBXModelData.size()-1].mMaterialMap);

	}


	for (int i = 0; i < node->GetChildCount(); ++i)
	{
		ReadMeshNodeData(node->GetChild(i));
	}
}

void ImportFBX::ProcessMesh(FbxMesh* mesh, MemFBXModelData* fbxModel)
{

	int triangleCount = mesh->GetPolygonCount();
	int vertexCounter = 0;

	vector<Triangle>& mTriangleData = fbxModel->mTriangleData;

	for (int i = 0; i < triangleCount; ++i)
	{
		
		Triangle triangle;

		for (int j = 0; j < 3; ++j)
		{

			int ctrlPointIndex = mesh->GetPolygonVertex(i, j);

			ReadVertexPos(mesh, ctrlPointIndex, &triangle.vertexs[j].pos);

			ReadVertexNormal(mesh, ctrlPointIndex, vertexCounter, &triangle.vertexs[j].normal);

			ReadVertexColor(mesh, ctrlPointIndex, vertexCounter, &triangle.vertexs[j].color);

			//ReadVertexTangent(mesh, ctrlPointIndex, vertexCounter, &triangle.vertexs[j].tangent);

			ReadVertexUV(mesh, ctrlPointIndex, mesh->GetTextureUVIndex(i, j), &triangle.vertexs[j].uv);

			++vertexCounter;
		}

		CalculateTriangleTangent(triangle);

		mTriangleData.push_back(triangle);
	}

	ReadMeshMaterialIndex(mesh,mTriangleData);
}


//https://www.cnblogs.com/wantnon/p/4372764.html
void ImportFBX::ReadVertexPos(FbxMesh* mesh, int ctrlPointIndex, XMFLOAT3* pos)
{
	FbxNode* meshNode = mesh->GetNode();
	FbxAnimEvaluator* lEvaluator = mScene->GetAnimationEvaluator();
	FbxMatrix lGlobal;

	lGlobal.SetIdentity();
	lGlobal = lEvaluator->GetNodeGlobalTransform(meshNode);
	FbxDouble3 scaling = meshNode->LclScaling.Get();
	FbxVector4 * ctrPoints = mesh->GetControlPoints();
	pos->x = (float)(ctrPoints[ctrlPointIndex][0] * scaling[0]);
	pos->y = (float)(ctrPoints[ctrlPointIndex][2] * scaling[2]);
	pos->z = (float)(-ctrPoints[ctrlPointIndex][1] * scaling[1]);


}

void ImportFBX::ReadVertexColor(FbxMesh* mesh, int ctrlPointIndex, int vertexCount, XMFLOAT3* color)
{
	if (mesh->GetElementVertexColorCount() < 1)
	{
		return;
	}

	FbxGeometryElementVertexColor* vertexColor = mesh->GetElementVertexColor();

	switch (vertexColor->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
	{
		switch (vertexColor->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			color->x = (float)vertexColor->GetDirectArray().GetAt(ctrlPointIndex).mRed;
			color->y = (float)vertexColor->GetDirectArray().GetAt(ctrlPointIndex).mGreen;
			color->z = (float)vertexColor->GetDirectArray().GetAt(ctrlPointIndex).mBlue;
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int id = vertexColor->GetIndexArray().GetAt(ctrlPointIndex);
			color->x = (float)vertexColor->GetDirectArray().GetAt(id).mRed;
			color->y = (float)vertexColor->GetDirectArray().GetAt(id).mGreen;
			color->z = (float)vertexColor->GetDirectArray().GetAt(id).mBlue;
		}
		break;

		default:
			break;
		}
	}
	break;

	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (vertexColor->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			color->x = (float)vertexColor->GetDirectArray().GetAt(vertexCount).mRed;
			color->y = (float)vertexColor->GetDirectArray().GetAt(vertexCount).mGreen;
			color->z = (float)vertexColor->GetDirectArray().GetAt(vertexCount).mBlue;
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int id = vertexColor->GetIndexArray().GetAt(vertexCount);
			color->x = (float)vertexColor->GetDirectArray().GetAt(id).mRed;
			color->y = (float)vertexColor->GetDirectArray().GetAt(id).mGreen;
			color->z = (float)vertexColor->GetDirectArray().GetAt(id).mBlue;
		}
		break;

		default:
			break;
		}
		break;
	}
	}

}


void ImportFBX::ReadVertexNormal(FbxMesh* mesh, int ctrlPointIndex, int vertexCount, XMFLOAT3* normal)
{
	if (mesh->GetElementNormalCount() < 1)
	{
		return;
	}

	FbxGeometryElementNormal* vertexNormal = mesh->GetElementNormal(0);

	switch (vertexNormal->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
	{
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			normal->x = (float)vertexNormal->GetDirectArray().GetAt(ctrlPointIndex).mData[0];
			normal->y = (float)vertexNormal->GetDirectArray().GetAt(ctrlPointIndex).mData[2];
			normal->z = (float)-vertexNormal->GetDirectArray().GetAt(ctrlPointIndex).mData[1];
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int id = vertexNormal->GetIndexArray().GetAt(ctrlPointIndex);
			normal->x = (float)vertexNormal->GetDirectArray().GetAt(id).mData[0];
			normal->y = (float)vertexNormal->GetDirectArray().GetAt(id).mData[2];
			normal->z = (float)-vertexNormal->GetDirectArray().GetAt(id).mData[1];
		}
		break;

		default:
			break;
		}
	}
	break;

	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			normal->x = (float)vertexNormal->GetDirectArray().GetAt(vertexCount).mData[0];
			normal->y = (float)vertexNormal->GetDirectArray().GetAt(vertexCount).mData[2];
			normal->z = (float)-vertexNormal->GetDirectArray().GetAt(vertexCount).mData[1];
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int id = vertexNormal->GetIndexArray().GetAt(vertexCount);
			normal->x = (float)vertexNormal->GetDirectArray().GetAt(id).mData[0];
			normal->y = (float)vertexNormal->GetDirectArray().GetAt(id).mData[2];
			normal->z = (float)-vertexNormal->GetDirectArray().GetAt(id).mData[1];
		}
		break;

		default:
			break;
		}
		break;
	}
	}
}



void ImportFBX::ReadVertexTangent(FbxMesh* mesh, int ctrlPointIndex, int vertexCount, XMFLOAT3* tangent)
{
	if (mesh->GetElementTangentCount() < 1)
	{
		return;
	}

	FbxGeometryElementTangent* vertexTangent = mesh->GetElementTangent(0);

	switch (vertexTangent->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
	{
		switch (vertexTangent->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			tangent->x = (float)vertexTangent->GetDirectArray().GetAt(ctrlPointIndex).mData[0];
			tangent->y = (float)vertexTangent->GetDirectArray().GetAt(ctrlPointIndex).mData[2];
			tangent->z = (float)-vertexTangent->GetDirectArray().GetAt(ctrlPointIndex).mData[1];
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int id = vertexTangent->GetIndexArray().GetAt(ctrlPointIndex);
			tangent->x = (float)vertexTangent->GetDirectArray().GetAt(id).mData[0];
			tangent->y = (float)vertexTangent->GetDirectArray().GetAt(id).mData[2];
			tangent->z = (float)(-vertexTangent->GetDirectArray().GetAt(id).mData[1]);
		}
		break;

		default:
			break;
		}
	}
	break;

	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (vertexTangent->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			tangent->x = (float)(vertexTangent->GetDirectArray().GetAt(vertexCount).mData[0]);
			tangent->y = (float)(vertexTangent->GetDirectArray().GetAt(vertexCount).mData[2]);
			tangent->z = (float)(-vertexTangent->GetDirectArray().GetAt(vertexCount).mData[1]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int id = vertexTangent->GetIndexArray().GetAt(vertexCount);
			tangent->x = (float)(vertexTangent->GetDirectArray().GetAt(id).mData[0]);
			tangent->y = (float)(vertexTangent->GetDirectArray().GetAt(id).mData[2]);
			tangent->z = (float)(-vertexTangent->GetDirectArray().GetAt(id).mData[1]);
		}
		break;

		default:
			break;
		}
		break;
	}
	}
}


void ImportFBX::ReadVertexUV(FbxMesh* mesh, int ctrlPointIndex, int uvIndex, XMFLOAT2* uv)
{
	if (mesh->GetElementUVCount() < 1)
	{
		return;
	}

	FbxGeometryElementUV* vertexUV = mesh->GetElementUV(0);

	switch (vertexUV->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
	{
		switch (vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			uv->x = (float)(vertexUV->GetDirectArray().GetAt(ctrlPointIndex).mData[0]);
			uv->y = (float)(1.0f - vertexUV->GetDirectArray().GetAt(ctrlPointIndex).mData[1]);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int id = vertexUV->GetIndexArray().GetAt(ctrlPointIndex);
			uv->x = (float)(vertexUV->GetDirectArray().GetAt(id).mData[0]);
			uv->y = (float)(1.0f - vertexUV->GetDirectArray().GetAt(id).mData[1]);
		}
		break;

		default:
			break;
		}
	}
	break;

	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		case FbxGeometryElement::eIndexToDirect:
		{
			uv->x = (float)vertexUV->GetDirectArray().GetAt(uvIndex).mData[0];
			uv->y = (float)(1.0f - vertexUV->GetDirectArray().GetAt(uvIndex).mData[1]);
		}
		break;

		default:
			break;
		}
		break;
	}
	}
}


//FbxLayeredTexture
void ImportFBX::LoadMaterialTexture(FbxSurfaceMaterial* surfaceMaterial,
	int materialIndex, map<int, MaterialTexFileName>& materialMap)
{

	FbxProperty pProperty;

	//DiffuseMap
	pProperty = surfaceMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
	ReadReletiveTextureFileName(&pProperty, materialIndex,materialMap);

	//SpecularMap
	pProperty = surfaceMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
	ReadReletiveTextureFileName(&pProperty, materialIndex, materialMap);

	//alphaTexture
	pProperty = surfaceMaterial->FindProperty(FbxSurfaceMaterial::sTransparentColor);
	ReadReletiveTextureFileName(&pProperty, materialIndex, materialMap);

	//bumpMap
	pProperty = surfaceMaterial->FindProperty(FbxSurfaceMaterial::sBump);
	ReadReletiveTextureFileName(&pProperty, materialIndex, materialMap);

}

void ImportFBX::ReadReletiveTextureFileName(FbxProperty* mproperty,
	int materialIndex, map<int, MaterialTexFileName>& materialMap)
{
	if (!mproperty || !mproperty->IsValid())
	{
		return;
	}
	string name = mproperty->GetName();
	bool isNeedTexture = false;
	isNeedTexture = (name == FbxSurfaceMaterial::sDiffuse) || (name == FbxSurfaceMaterial::sSpecular)
		|| (name == FbxSurfaceMaterial::sTransparentColor) || (name == FbxSurfaceMaterial::sBump);
	if (!isNeedTexture)
	{
		return;
	}


	int textureNum = mproperty->GetSrcObjectCount<FbxFileTexture>();
	if (textureNum > 0)
	{
		FbxFileTexture* fbxFileTexture = mproperty->GetSrcObject<FbxFileTexture>(0);
		string  relativeFileName = fbxFileTexture->GetRelativeFileName();
		size_t tgaTagPos = relativeFileName.find(".tga");
		if (tgaTagPos != string::npos)
		{
			relativeFileName = relativeFileName.substr(0, tgaTagPos);
			relativeFileName += string(".dds");
		}
		string fileName = fbxFileNamePre + relativeFileName;
		if (name == FbxSurfaceMaterial::sDiffuse)
		{
			materialMap[materialIndex].diffuseMapFileName = fileName;
		}
		else if (name == FbxSurfaceMaterial::sSpecularFactor)
		{
			materialMap[materialIndex].specularMapFileName = fileName;
		}
		else if (name == FbxSurfaceMaterial::sTransparentColor)
		{
			materialMap[materialIndex].alphaMapFileName = fileName;
		}
		else if (name == FbxSurfaceMaterial::sBump)
		{
			materialMap[materialIndex].bumpMapFileName = fileName;
		}

	}

}

void ImportFBX::ReadMeshMaterialIndex(FbxMesh* mesh, vector<Triangle>& triangleData)
{
	int ltriangleCount = mesh->GetPolygonCount();

	fbxsdk::FbxLayerElementArrayTemplate<int>* pMaterialIndices;
	FbxGeometryElement::EMappingMode materialMappingMode = FbxGeometryElement::eNone;

	if (mesh->GetElementMaterialCount()>0)
	{
		pMaterialIndices = &mesh->GetElementMaterial()->GetIndexArray();
		materialMappingMode = mesh->GetElementMaterial()->GetMappingMode();
		if (pMaterialIndices)
		{

			switch (materialMappingMode)
			{

			case fbxsdk::FbxLayerElement::eByPolygon:
			{
				if (pMaterialIndices->GetCount() == ltriangleCount)
				{
					for (int triangleIndex = 0; triangleIndex < ltriangleCount; ++triangleIndex)
					{
						int materialIndex = pMaterialIndices->GetAt(triangleIndex);
						triangleData[triangleIndex].MaterialId = materialIndex;
					}
				}
			}
			break;

			case fbxsdk::FbxLayerElement::eAllSame:
			{
				int materialIndex = pMaterialIndices->GetAt(0);
				for (int triangleIndex = 0; triangleIndex < ltriangleCount; ++triangleIndex)
				{
					triangleData[triangleIndex].MaterialId = materialIndex;
				}
			}
			break;
			default:
				break;
			}
		}
	}
}

void ImportFBX::LoadMaterial(FbxMesh* mesh, map<int, MaterialTexFileName>& materialMap)
{
	int materialCount;
	FbxNode* pNode;

	if (mesh&&mesh->GetNode())
	{
		pNode = mesh->GetNode();
		materialCount = pNode->GetMaterialCount();
	}
	else
	{
		return;
	}
	if (materialCount > 0)
	{
		for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex)
		{
			FbxSurfaceMaterial* surfaceMaterial = pNode->GetMaterial(materialIndex);
			LoadMaterialTexture(surfaceMaterial, materialIndex, materialMap);
		}
	}
}


void ImportFBX::ChangeModelData(ModelData* destModel, MemFBXModelData* srcMemFBXModel)
{
	destModel->mMaterialMap = srcMemFBXModel->mMaterialMap;

	map<int, MaterialTexFileName>& mMaterialMap = destModel->mMaterialMap;
	vector<Triangle>& mTriangleData = srcMemFBXModel->mTriangleData;
	
	bool isExitTexture = true;

	if (destModel->mMaterialMap.size() == 0)
	{
		isExitTexture = false;
		destModel->mMeshList.resize(1);
		destModel->mMeshList[0].materialId = -1; 
	}
	else
	{
		destModel->mMeshList.resize(destModel->mMaterialMap.size());
	}
	int i = 0;

	if (isExitTexture)
	{

		for (auto it = mMaterialMap.begin(); it != mMaterialMap.end(); it++)
		{
			destModel->mMeshList[i].materialId = it->first;
			++i;
		}

		for (int triangleIndex = 0; triangleIndex < (int)mTriangleData.size(); ++triangleIndex)
		{
			int meshIndex = 0;

			for (meshIndex = 0; meshIndex < (int)destModel->mMeshList.size(); ++meshIndex)
			{
				if (destModel->mMeshList[meshIndex].materialId == mTriangleData[triangleIndex].MaterialId)
				{
					break;
				}
			}

			if (meshIndex == destModel->mMeshList.size())
			{
				meshIndex = (int)destModel->mMeshList.size() - 1;
			}

			for (int vertexIndex = 0; vertexIndex < 3; ++vertexIndex)
			{
				VertexPCNTT vertex = mTriangleData[triangleIndex].vertexs[vertexIndex];

				destModel->mMeshList[meshIndex].mVertexData.push_back(vertex);
			}
		}
	}
	else
	{
		for (int triangleIndex = 0; triangleIndex < (int)mTriangleData.size(); ++triangleIndex)
		{
			for (int vertexIndex = 0; vertexIndex < 3; ++vertexIndex)
			{
				VertexPCNTT vertex = mTriangleData[triangleIndex].vertexs[vertexIndex];

				destModel->mMeshList[0].mVertexData.push_back(vertex);
			}
		}
	}
	
	for (int j = 0; j < (int)destModel->mMeshList.size(); ++j)
	{
		int vertexNum = (int)destModel->mMeshList[j].mVertexData.size();
		destModel->mMeshList[j].mIndexData.resize(vertexNum);
		for (int s = 0; s < vertexNum; ++s)
		{
			destModel->mMeshList[j].mIndexData[s] = s;
		}
	}
}

void ImportFBX::ClearMemFBXModelData()
{

	mMemFBXModelData.clear();


}

void ImportFBX::TriangulateRecursive(FbxNode* node)
{
	FbxNodeAttribute* lNodeAttribute = node->GetNodeAttribute();

	if (lNodeAttribute)
	{
		if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			FbxGeometryConverter lConverter(node->GetFbxManager());
			lConverter.Triangulate(mScene, true);
		}
	}

	const int lChildCount = node->GetChildCount();
	for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
	{
		TriangulateRecursive(node->GetChild(lChildIndex));
	}
}

void ImportFBX::CalculateTriangleTangent(Triangle& triangle)
{
	float edge1[3], edge2[3];
	float texEdge1[2], texEdge2[2];
	VertexPCNTT  &vertex1 = triangle.vertexs[0];
	VertexPCNTT  &vertex2 = triangle.vertexs[1];
	VertexPCNTT  &vertex3 = triangle.vertexs[2];

	edge1[0] = vertex2.pos.x - vertex1.pos.x; //E0X  
	edge1[1] = vertex2.pos.y - vertex1.pos.y; //E0Y  
	edge1[2] = vertex2.pos.z - vertex1.pos.z; //E0Z  
								
	edge2[0] = vertex3.pos.x - vertex1.pos.x; //E1X  
	edge2[1] = vertex3.pos.y - vertex1.pos.y; //E1Y  
	edge2[2] = vertex3.pos.z - vertex1.pos.z; //E1Z  

	texEdge1[0] = vertex2.uv.x - vertex1.uv.x; //U0  
	texEdge1[1] = vertex2.uv.y - vertex1.uv.y; //V0  

	texEdge2[0] = vertex3.uv.x - vertex1.uv.x; //U1  
	texEdge2[1] = vertex3.uv.y - vertex1.uv.y; //V1  
									 
	float den = 1.0f / (texEdge1[0] * texEdge2[1] - texEdge1[1] * texEdge2[0]);

	XMFLOAT3 tangent;
	tangent.x = den*(texEdge2[1] * edge1[0] - texEdge1[1] * edge2[0]);
	tangent.y = den*(texEdge2[1] * edge1[1] - texEdge1[1] * edge2[1]);
	tangent.z = den*(texEdge2[1] * edge1[2] - texEdge1[1] * edge2[2]);

	float length = sqrt(tangent.x*tangent.x + tangent.y*tangent.y + tangent.z*tangent.z);
	tangent.x /= length;
	tangent.y /= length;
	tangent.z /= length;

	//Tangent
	for (int i = 0; i < 3; ++i)
	{
		triangle.vertexs[i].tangent = tangent;
	}
	
}

shared_ptr<ImportFBX> ImportFBX::m_pImportFBX = nullptr;