#pragma once
#include "entt.hpp"
#include <string>
#include <DirectXMath.h>
using namespace DirectX;
using std::string;
class CameraObject;
class Actor;

class Scene
{
public:
	Scene();
	~Scene();

	void OnTickEditor(float deltaTime, CameraObject* camera);
	void OnTickRuntime(float deltaTime);
	entt::registry& GetResgtry();
	Actor CreateActor(const string& actorName = "");
	void Destroy(Actor actor);
	XMMATRIX GetMainViewMatrix();
	XMMATRIX GetMainProjectMatrix();

private:
	void OnUpdateScript(float deltaTime);
	void OnUpdateCamera();
	void OnRender();
	CameraObject* mainCamera;

private:
	entt::registry coreRegistry;
	friend class SceneHierarchyPanel;
};