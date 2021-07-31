#include "EditorStyle.h"


EditorStyle::EditorStyle()
{
	Init();
}

void EditorStyle::Init()
{
	ImGuiIO& io = ImGui::GetIO();

	//Fonts
	regularFont = io.FontDefault = io.Fonts->AddFontFromFileTTF("content/fonts/Roboto-Regular.ttf", 18.0f);
	boldFont = io.Fonts->AddFontFromFileTTF("content/fonts/Roboto-Bold.ttf", 18.0f);
	
	//Style Colors
	SetupStyleColor();
}

shared_ptr<EditorStyle> EditorStyle::Get()
{
	if (!single)
		single = make_shared<EditorStyle>();

	return single;
}

void EditorStyle::SetupStyleColor()
{
	/*
	UE5 Style
	*/
	
	auto& StyleColors = ImGui::GetStyle().Colors;

	//Windows Bg
	StyleColors[ImGuiCol_WindowBg] = ImVec4(0.066f, 0.066f, 0.070f, 1.0f);

	//Headers
	StyleColors[ImGuiCol_Header] = ImVec4(0.192f, 0.180f, 0.180f, 1.0f);
}

shared_ptr<EditorStyle> EditorStyle::single = nullptr;