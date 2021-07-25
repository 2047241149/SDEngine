#include "SceneHierarchyPanel.h"
#include "imgui/imgui.h"
#include "../ImguiUtil.h"
#include "SDEngine/Scene/Scene.h"
#include "SDEngine/Scene/TagComponent.h"
#include "SDEngine/Scene/TransformComponent.h"
#include "SDEngine/Scene/CameraComponent.h"
#include "SDEngine/Scene/MeshComponent.h"
#include "SDEngine/Shader/Material.h"

SceneHierarchyPanel::SceneHierarchyPanel(shared_ptr<Scene> inScene)
{
	SetScene(inScene);
}

void SceneHierarchyPanel::SetScene(shared_ptr<Scene> inScene)
{
	scene = inScene;
}

void SceneHierarchyPanel::OnImguiRender()
{
	ImGui::Begin("World Outline");
	scene->coreRegistry.each([this](auto enity)
	{
		Actor actor(enity, scene.get());
		DrawActorNode(actor);
	});

	if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
	{
		selectActor = {};
	}

	ImGui::End();


	ImGui::Begin("Details");
	if (selectActor.IsValid())
	{
		DrawActorDeail(selectActor);
	}

	ImGui::End();
}

void SceneHierarchyPanel::DrawActorNode(Actor actor)
{
	auto& tagCpt = actor.GetComponent<TagComponent>();

	ImGuiTreeNodeFlags flag = (selectActor == actor ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
	void* ptr_id = (void*)(actor.GetEntity());
	bool bOpen = ImGui::TreeNodeEx(ptr_id, flag, tagCpt.tag.c_str());
	if (ImGui::IsItemClicked())
	{
		selectActor = actor;
	}

	if (bOpen)
	{
		ImGui::TreePop();
	}
}

void SceneHierarchyPanel::DrawActorDeail(Actor actor)
{
	if (actor.HasComponent<TagComponent>())
	{
		auto& tagCpt = actor.GetComponent<TagComponent>();
		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, tagCpt.tag.c_str());
		if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
		{
			tagCpt.tag = string(buffer);
		}
	}

	if (actor.HasComponent<TransformComponent>())
	{
		auto& transformCpt = actor.GetComponent<TransformComponent>();
		if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
		{
			ImGui::DragXMFloat3("position", &transformCpt.position, 0.1f);
			ImGui::TreePop();
		}

		if (actor.HasComponent<CameraComponent>())
		{
			auto& cameraCpt = actor.GetComponent<CameraComponent>();
			if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
			{
				ImGui::DragFloat("fov", &cameraCpt.camera.fovY, 0.1f, 0.001f, 10.0f);
				ImGui::DragFloat("aspect", &cameraCpt.camera.aspect, 0.1f, 0.001f, 10.0f);
				ImGui::DragFloat("nearPlane", &cameraCpt.camera.nearPlane, 1.0f, 0.001f, 10000.0f);
				ImGui::DragFloat("farPlane", &cameraCpt.camera.farPlane, 1.0f, 0.001f, 10000.0f);
				ImGui::TreePop();
			}

			cameraCpt.camera.position = transformCpt.position;
			cameraCpt.camera.UpdateViewMatrix();
		}
	}

	if (actor.HasComponent<MeshComponent>())
	{
		auto& meshCpt = actor.GetComponent<MeshComponent>();
		if (ImGui::TreeNodeEx((void*)typeid(MeshComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Mesh"))
		{
			ImGui::Checkbox("bTransparent", &meshCpt.bTransparent);
			ImGui::Checkbox("bReflect", &meshCpt.bReflect);
			ImGui::Checkbox("bCastShadow", &meshCpt.bCastShadow);

			// material attri
			shared_ptr<Material> mat = meshCpt.GetMaterial();
			if (mat)
			{
				for (auto& it : mat->materialValues)
				{
					if (it.second)
					{
						switch (it.second->type)
						{
						case SHADER_FLOAT:
							ImGui::DragFloat(it.first.c_str(), &it.second->value.floatValue, 0.001f);
							break;

						case SHADER_FLOAT2:
							ImGui::DragXMFloat2(it.first.c_str(), &it.second->value.float2Value, 0.001f);
							break;

						case SHADER_FLOAT3:
							ImGui::ColorEditXMFloat3(it.first.c_str(), &it.second->value.float3Value, ImGuiColorEditFlags_Float);
							break;

						case SHADER_FLOAT4:
							ImGui::ColorEditXMFloat4(it.first.c_str(), &it.second->value.float4Value, ImGuiColorEditFlags_Float);
							break;
						} 
					}
				}
			}

			ImGui::TreePop();
		}
	
	}
}