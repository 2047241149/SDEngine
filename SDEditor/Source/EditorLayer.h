#pragma once

#include "Engine.h"
#include "imgui/imgui.h"

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
	entt::entity actor;

public:
	EditorLayer();

private:
	void UpdateCamera(float deltaTime);
	bool bViewportFouces = true;
	bool bViewportHover = true;

public:
	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(float deltaTime) override;
	void OnImguiRender() override;
	void OnEvent(Event& event) override;
	void End() override;
};