#pragma once
#include "SDEngine/Scene/Actor.h"
#include <memory>
using std::shared_ptr;
class Scene;

class SceneHierarchyPanel
{
public:
	SceneHierarchyPanel() = default;
	SceneHierarchyPanel(shared_ptr<Scene> inScene);
	void SetScene(shared_ptr<Scene> inScene);
	void OnImguiRender();

private:
	void DrawActorNode(Actor actor);
	void DrawActorDeail(Actor actor);

private:
	shared_ptr<Scene> scene;
	Actor selectActor;
};