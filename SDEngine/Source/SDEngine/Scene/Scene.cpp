#include "Scene.h"
#include "Actor.h"
#include "Render/Renderer.h"
#include "TransformComponent.h"
#include "MeshComponent.h"
#include "TagComponent.h"
#include "CameraComponent.h"

Scene::Scene()
{
	mainCamera = nullptr;
}

Scene::~Scene()
{
}

void Scene::OnUpdate()
{
	OnUpdateCamera();
	OnRender();
}

void Scene::OnUpdateCamera()
{
	auto group = coreRegistry.view<TransformComponent, CameraComponent>();
	for (auto actor : group)
	{
		auto& [transform, camera] = group.get<TransformComponent, CameraComponent>(actor);

		if (camera.bPrimary)
		{
			mainCamera = &camera.camera;
			break;
		}
	}
}

void Scene::OnRender()
{
	if (nullptr != mainCamera)
	{
		GDirectxCore->UpdateRenderParams(mainCamera);
		auto group = coreRegistry.group<TransformComponent>(entt::get<MeshComponent>);
		for (auto& actor : group)
		{
			auto&[transform, mesh] = group.get<TransformComponent, MeshComponent>(actor);
			RendererContext::DrawMesh(&mesh, &transform);
		}
	}
}

entt::registry& Scene::GetResgtry()
{
	return coreRegistry;
}

Actor Scene::CreateActor(const string& actorName)
{
	entt::entity handle = coreRegistry.create();
	Actor actor(handle, this);
	const string& tagName = actorName.empty() ? "actor" : actorName;
	actor.AddComponent<TagComponent>(tagName);
	actor.AddComponent<TransformComponent>(XMFLOAT3(0.0f, 0.0f, 0.0f));
	return actor;
}