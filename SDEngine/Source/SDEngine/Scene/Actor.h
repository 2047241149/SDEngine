#pragma once
#include "Scene.h"
#include "entt.hpp"

class Actor
{
public:
	Actor();
	Actor(Actor& other) = default;
	Actor(entt::entity inHandle, Scene* inScene);

	template<typename T>
	bool HasComponent() const
	{
		return scene->GetResgtry().all_of<T>(handle);
	}

	template<typename T, typename... Args>
	T& AddComponent(Args&&... args)
	{
		return scene->GetResgtry().emplace<T>(handle, std::forward<Args>(args)...);
	}

	template<typename T>
	T& GetComponent()
	{
		return scene->GetResgtry().get<T>(handle);
	}

	template<typename T>
	T& GetOrAddComponent()
	{
		if (HasComponent<T>())
			return GetComponent<T>();

		return AddComponent<T>();
	}

	template<typename T>
	void RemoveComponnt()
	{
		scene->GetResgtry().remove<T>(handle);
	}

	operator bool() const 
	{
		return handle != entt::null;
	}

	bool IsValid()
	{
		return scene != nullptr && scene->GetResgtry().valid(handle);
	}

	bool operator == (const Actor& other) const
	{
		return handle == other.handle && scene == other.scene;
	}

	bool operator != (const Actor& other) const
	{
		return !(*this == other);
	}

	operator entt::entity() {
		return handle;
	}

	operator uint32_t() const {
		return (uint32_t)handle;
	}

	entt::entity GetEntity() { return handle; }

private:
	entt::entity handle = entt::null;
	Scene* scene;
};