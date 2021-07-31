#include "Scene.h"
#include "Render/Renderer.h"
#include "TransformComponent.h"
#include "MeshComponent.h"
#include "TagComponent.h"
#include "CameraComponent.h"
#include "ScriptComponent.h"
#include "Actor.h"

Scene::Scene()
{
	mainCamera = nullptr;
}

Scene::~Scene()
{
}

void Scene::OnTickEditor(float deltaTime, CameraObject* camera)
{
	mainCamera = camera;
	OnRender();
}

void Scene::OnTickRuntime(float deltaTime)
{
	OnUpdateScript(deltaTime);
	OnUpdateCamera();
	OnRender();
}

void Scene::OnUpdateScript(float deltaTime)
{
	auto& group = coreRegistry.view<ScriptComponent>();
	for (auto entity : group)
	{
		auto& scriptCpt = group.get<ScriptComponent>(entity);
		if (nullptr == scriptCpt.instance)
		{
			if (scriptCpt.CreateInstanceFunc)
			{
				scriptCpt.instance = scriptCpt.CreateInstanceFunc();
				scriptCpt.instance->SetActor(Actor(entity, this));
				scriptCpt.instance->BeginPlay();
			}
		}

		if (nullptr != scriptCpt.instance && scriptCpt.bCanTick)
		{
			scriptCpt.Tick(deltaTime);
		}
	}
}

void Scene::OnUpdateCamera()
{
	auto group = coreRegistry.view<TransformComponent, CameraComponent>();

	for (auto entity : group)
	{
		auto& [transform, camera] = group.get<TransformComponent, CameraComponent>(entity);

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
		for (auto& entity : group)
		{
			auto&[transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
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

void Scene::Destroy(Actor actor)
{
	coreRegistry.destroy(actor);
}