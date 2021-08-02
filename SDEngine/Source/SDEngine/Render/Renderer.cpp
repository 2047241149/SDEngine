#include "Renderer.h"
#include "Scene/MeshComponent.h"
#include "Scene/TransformComponent.h"
#include "Shader/Material.h"
#include "GameObject/Camera.h"
#include "Log.h"

void RendererContext::DrawMesh(MeshComponent* mesh, TransformComponent* transform)
{
	shared_ptr<Material> material = mesh->GetMaterial();
	if (!material)
	{
		material = Get()->nullMaterial;
	}

	material->SetMatrix("World", transform->GetWorldMatrix());
	material->SetMatrix("View", GDirectxCore->viewParams->viewMatrix);
	material->SetMatrix("Proj", GDirectxCore->viewParams->projMatrix);
	material->Apply();

	g_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	if (!mesh->m_pFBXModel)
	{
		Log::Error("FBXModel is NULL");
		return;
	}

	vector<ModelData>& mModelList = mesh->m_pFBXModel->mModelList;
	for (UINT index = 0; index < mModelList.size(); ++index)
	{
		ModelData* mModelData = &mModelList[index];
		vector<MeshData>& mMeshList = mModelData->mMeshList;

		for (UINT i = 0; i < mMeshList.size(); ++i)
		{
			MeshData& mesh = mMeshList[i];
			UINT stride = sizeof(mesh.mVertexData[0]);
			UINT offset = 0;
			g_pDeviceContext->IASetVertexBuffers(0, 1, &mesh.mVertexBuffer, &stride, &offset);
			g_pDeviceContext->IASetIndexBuffer(mesh.mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
			GDirectxCore->DrawIndexed((UINT)mesh.mIndexData.size(), 0, 0);
		}
	}
}

RendererContext* RendererContext::Get()
{
	if (nullptr == instance)
	{
		instance = make_shared<RendererContext>();
		instance->Init();
	}

	return instance.get();
}

void RendererContext::Init()
{
	shared_ptr<VertexPixelShader> nullShader = make_shared<VertexPixelShader>("Content/NullShader.fx");
	nullMaterial = make_shared<Material>(nullShader);
}

shared_ptr<RendererContext> RendererContext::instance = nullptr;