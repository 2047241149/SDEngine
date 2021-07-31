#pragma once
#include <iostream>
#include <memory>
#include "imgui/imgui.h"
using namespace std;

class EditorStyle
{
public:
	EditorStyle();
	static shared_ptr<EditorStyle> Get();

public:
	void Init();

public:
	ImFont* regularFont;
	ImFont* boldFont;

private:
	static shared_ptr<EditorStyle> single;

private:
	void SetupStyleColor();
};

#define GEditorStyle (EditorStyle::Get())