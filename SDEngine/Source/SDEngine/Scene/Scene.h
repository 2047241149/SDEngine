#pragma once
#include "entt.hpp"
#include <string>

using std::string;
class Actor;
class CameraObject;

class Scene
{
public:
	Scene();
	~Scene();

	void OnUpdate();
	entt::registry& GetResgtry();
	Actor CreateActor(const string& actorName = "");

private:
	void OnUpdateCamera();
	void OnRender();
	CameraObject* mainCamera;

private:
	entt::registry coreRegistry;
};