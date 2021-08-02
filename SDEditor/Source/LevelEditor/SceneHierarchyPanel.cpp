#include "SceneHierarchyPanel.h"
#include "imgui/imgui.h"
#include "../ImguiUtil.h"
#include "SDEngine/Scene/Scene.h"
#include "SDEngine/Scene/TagComponent.h"
#include "SDEngine/Scene/TransformComponent.h"
#include "SDEngine/Scene/CameraComponent.h"
#include "SDEngine/Scene/MeshComponent.h"
#include "SDEngine/Shader/Material.h"
#include "SDEngine/Shader/ShaderLibrary.h"

SceneHierarchyPanel::SceneHierarchyPanel(shared_ptr<Scene> inScene)
{
	SetScene(inScene);
}

void SceneHierarchyPanel::SetScene(shared_ptr<Scene> inScene)
{
	scene = inScene;
	selectActor = {};
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


	//Create Actor UI
	if (ImGui::BeginPopupContextWindow(0, 1, false))
	{
		if (ImGui::MenuItem("Create Actor"))
			scene->CreateActor("actor");

		ImGui::EndPopup();
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
	flag |= ImGuiTreeNodeFlags_SpanAvailWidth;
	void* ptr_id = (void*)(actor.GetEntity());
	bool bOpen = ImGui::TreeNodeEx(ptr_id, flag, tagCpt.tag.c_str());
	if (ImGui::IsItemClicked())
	{
		selectActor = actor;
	}

	//Delete Actor UI
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Delete Actor"))
		{
			scene->Destroy(actor);
			selectActor = {};
		}

		ImGui::EndPopup();
	}

	if (bOpen)
	{
		ImGui::TreePop();
	}
}

template<typename T, typename UIFunction>
static void DrawComponent(const string name, Actor actor, UIFunction function)
{
	if (actor.HasComponent<T>())
	{
		auto& cpt = actor.GetComponent<T>();
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		//ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.192, 0.180, 0.180f, 1.0f));
		ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap;
		bool bOpen = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), flag, name.c_str());
		float lineHeight = GImGui->Font->FontSize + 2.0 * GImGui->Style.FramePadding.y;
		ImGui::SameLine(ImGui::GetWindowWidth() - lineHeight);
		if (ImGui::Button("+", ImVec2(20, 20)))
		{
			ImGui::OpenPopup("ComponentSettings");
		}

		ImGui::PopStyleVar();
		//ImGui::PopStyleColor();
		bool removeComponent = false;

		if (ImGui::BeginPopup("ComponentSettings"))
		{
			if (ImGui::MenuItem("Remove Component"))
				removeComponent = true;

			ImGui::EndPopup();
		}

		if (bOpen)
		{
			function(cpt);
			ImGui::TreePop();
		}

		if (removeComponent)
			actor.RemoveComponnt<T>();
	}
}

void SceneHierarchyPanel::DrawActorDeail(Actor actor)
{
	if (actor.HasComponent<TagComponent>())
	{
		auto& tagCpt = actor.GetComponent<TagComponent>();
		char buffer[128];
		memset(buffer, 0, sizeof(buffer));
		strcpy_s(buffer, tagCpt.tag.c_str());
		if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
		{
			tagCpt.tag = string(buffer);
		}
	}

	ImGui::SameLine();
	ImGui::PushItemWidth(-1);

	if (ImGui::Button("Add"))
		ImGui::OpenPopup("AddComponent");

	if (ImGui::BeginPopup("AddComponent"))
	{
		if (ImGui::MenuItem("Camera") && !selectActor.HasComponent<CameraComponent>())
			selectActor.AddComponent<CameraComponent>();

		if (ImGui::MenuItem("Mesh") && !selectActor.HasComponent<MeshComponent>())
		{
			selectActor.AddComponent<MeshComponent>("Content\\sphere.FBX");
		}

		ImGui::EndPopup();
	}

	ImGui::PopItemWidth();

	auto& transformCpt = actor.GetComponent<TransformComponent>();
	DrawComponent<TransformComponent>("Transform", actor, [](auto& cpt)
	{
		ImGui::DragXMFloat3Control("position", &cpt.position);
		ImGui::DragXMFloat3Control("Rotation", &cpt.rotation);
		ImGui::DragXMFloat3Control("Scale", &cpt.scale, 1.0f);
	});

	DrawComponent<CameraComponent>("Camera", actor, [&transformCpt](auto& cpt)
	{
		ImGui::DragFloat("fov", &cpt.camera.fovY, 0.1f, 0.001f, 10.0f);
		ImGui::DragFloat("aspect", &cpt.camera.aspect, 0.1f, 0.001f, 10.0f);
		ImGui::DragFloat("nearPlane", &cpt.camera.nearPlane, 1.0f, 0.001f, 10000.0f);
		ImGui::DragFloat("farPlane", &cpt.camera.farPlane, 1.0f, 0.001f, 10000.0f);
		cpt.camera.position = transformCpt.position;
		cpt.camera.UpdateViewMatrix();
	});

	DrawComponent<MeshComponent>("Mesh", actor, [](auto& cpt)
	{
		ImGui::Checkbox("bTransparent", &cpt.bTransparent);
		ImGui::Checkbox("bReflect", &cpt.bReflect);
		ImGui::Checkbox("bCastShadow", &cpt.bCastShadow);

		// material attri
		shared_ptr<Material> mat = cpt.GetMaterial();
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
	});
}