#pragma once
#include "Scene.h"
#include "entt.hpp"

class Actor
{
public:
	Actor(entt::entity inHandle, Scene* inScene);

	template<typename T>
	bool HasComponent()
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
	void RemoveComponnt()
	{
		scene->GetResgtry().remove<T>(handle);
	}

	operator bool() const 
	{
		return handle != entt::null;
	}



private:
	entt::entity handle = entt::null;
	Scene* scene;
};