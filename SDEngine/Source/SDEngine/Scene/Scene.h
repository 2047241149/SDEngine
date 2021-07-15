#pragma once
#include "entt.hpp"
#include<string>

using std::string;
class Actor;

class Scene
{
public:
	Scene();
	~Scene();

	void OnUpdate();
	entt::registry& GetResgtry();
	Actor CreateActor(const string& actorName = "");

private:
	void OnRender();

private:
	entt::registry coreRegistry;
};