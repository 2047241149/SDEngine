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

	void OnTick(float deltaTime);
	entt::registry& GetResgtry();
	Actor CreateActor(const string& actorName = "");

private:
	void OnUpdateScript(float deltaTime);
	void OnUpdateCamera();
	void OnRender();
	CameraObject* mainCamera;

private:
	entt::registry coreRegistry;
};