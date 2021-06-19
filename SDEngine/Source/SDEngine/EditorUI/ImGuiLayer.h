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
	//virtual void OnEvent(Event& event) override;
	void BeginRender();
	void EndRender();

private:
	void SetImguiKeyCode(UINT keyCode, int taregetValue);
	void UpdateMousePos();

public:
	//Keyborad event
	bool OnKeyPressedEvent(KeyPressedEvent& event);
	bool OnKeyReleasedEvent(KeyReleasedEvent& event);
	bool OnCharEvent(CharEvent& event);

	//Mouse event
	bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& event);
	bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& event);
	bool OnMouseMovedEvent(MouseMovedEvent& event);
	bool OnMouseScrollEvent(MouseScrollEvent& event);

	bool OnKillWindowFocus(KillFocusEvent& event);
};