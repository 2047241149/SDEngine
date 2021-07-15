#include "Scene.h"
#include "TransformComponent.h"
#include "MeshComponent.h"
#include "Render/Renderer.h"
#include "Actor.h"
#include "TagComponent.h"

Scene::Scene()
{
	/*entt::entity entity = coreRegistry.create();
	coreRegistry.emplace<TestComponent>(entity);
	TestComponent& test = coreRegistry.get<TestComponent>(entity);

	auto view = coreRegistry.view<TestComponent>();
	for (auto entity : view)
	{
		TestComponent& intest = coreRegistry.get<TestComponent>(entity);
	}*/
}

Scene::~Scene()
{

}

void Scene::OnUpdate()
{
	OnRender();
}

void Scene::OnRender()
{
	auto group = coreRegistry.group<TransformComponent>(entt::get<MeshComponent>);
	for(auto actor : group)
	{
		auto& [transform, mesh] = group.get<TransformComponent, MeshComponent>(actor);
		RendererContext::DrawMesh(&mesh, &transform);
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