#pragma once
#include <DirectXMath.h>
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include <string>
using namespace::DirectX;
using std::string;

namespace ImGui
{
	bool DragXMFloat2(const char* label, XMFLOAT2* value, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
	bool DragXMFloat3(const char* label, XMFLOAT3* value, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
	bool DragXMFloat4(const char* label, XMFLOAT4* value, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
	bool ColorEditXMFloat4(const char* label, XMFLOAT4* value, ImGuiColorEditFlags flags = 0);
	bool ColorEditXMFloat3(const char* label, XMFLOAT3* value, ImGuiColorEditFlags flags = 0);
	bool DragXMFloat3Control(const string& label, XMFLOAT3* value, float resetValue = 0.0f, float colimnWidth = 100.0f);
}