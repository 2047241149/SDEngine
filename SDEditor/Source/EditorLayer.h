#pragma once

#include "Engine.h"
#include "imgui/imgui.h"
#include "LevelEditor/SceneHierarchyPanel.h"
#include "SDEngine/GameObject/EditorCamera.h"

class EditorLayer : public Layer
{
private:
	float cameraMoveSpeed = 3.0f;
	float cameraRotateSpeed = 0.5f;
	shared_ptr<VertexPixelShader> pureColorShader;
	shared_ptr<Texture> baseDiffuse;
	shared_ptr<Material> material;
	float surfaceColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	shared_ptr<RenderTexture> rt;
	shared_ptr<Scene> scene;
	Actor meshActor;
	Actor meshActor1;
	Actor secondCameraActor;
	EditorCamera editorCamera;
	bool bUseEditorMode;
	int guizmoOperateType = -1;

private:
	SceneHierarchyPanel scenePanel;

public:
	EditorLayer();

private:
	void ShowMenuUI();
	void ShowDockSpaceUI();
	void ShowRenderStatisticsUI();
	void ShowGameWindowUI();

private:
	bool bViewportFouces = true;
	bool bViewportHover = true;

public:
	void OnAttach() override;
	void OnDetach() override;
	void OnTick(float deltaTime) override;
	void OnImguiRender() override;
	void OnEvent(Event& event) override;
	void End() override;

private:
	bool OnKeyPress(KeyPressedEvent& event);
	void CreateNewScene();
	void SaveScene();
};