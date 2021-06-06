#pragma once
#include "SDEngine/Layer.h"

class SD_API ImGuiLayer : public Layer
{
public:
	ImGuiLayer();
	~ImGuiLayer();

public:
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(float deltaTime) override;
	virtual void OnEvent(Event& event) override;
};