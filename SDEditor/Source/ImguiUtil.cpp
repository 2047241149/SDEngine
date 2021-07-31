#include "ImguiUtil.h"
#include "EditorStyle/EditorStyle.h"

namespace ImGui
{
	bool DragXMFloat2(const char* label, XMFLOAT2* v, float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
	{
		return DragScalarN(label, ImGuiDataType_Float, v, 2, v_speed, &v_min, &v_max, format, flags);
	}

	bool DragXMFloat3(const char* label, XMFLOAT3* v, float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
	{
		return DragScalarN(label, ImGuiDataType_Float, v, 3, v_speed, &v_min, &v_max, format, flags);
	}

	bool DragXMFloat4(const char* label, XMFLOAT4* v, float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
	{
		return DragScalarN(label, ImGuiDataType_Float, v, 4, v_speed, &v_min, &v_max, format, flags);
	}

	bool ColorEditXMFloat4(const char* label, XMFLOAT4* value, ImGuiColorEditFlags flags)
	{
		float tempValue[4];
		tempValue[0] = value->x;
		tempValue[1] = value->y;
		tempValue[2] = value->z;
		tempValue[3] = value->w;

		bool bEdit = ImGui::ColorEdit4(label, tempValue, flags);

		value->x = tempValue[0];
		value->y = tempValue[1];
		value->z = tempValue[2];
		value->w = tempValue[3];

		return bEdit;
	}

	bool ColorEditXMFloat3(const char* label, XMFLOAT3* value, ImGuiColorEditFlags flags)
	{
		flags = flags | ImGuiColorEditFlags_NoAlpha;
		float tempValue[3];
		tempValue[0] = value->x;
		tempValue[1] = value->y;
		tempValue[2] = value->z;

		bool bEdit = ImGui::ColorEdit4(label, tempValue, flags);

		value->x = tempValue[0];
		value->y = tempValue[1];
		value->z = tempValue[2];

		return bEdit;
	}

	bool DragXMFloat3Control(const string& label, XMFLOAT3* value, float resetValue, float colimnWidth)
	{
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, colimnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();
		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		//X
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
		ImGui::PushFont(GEditorStyle->boldFont);
		
		if (ImGui::Button("X", buttonSize))
			value->x = resetValue;

		ImGui::PopStyleColor(3);
		ImGui::PopFont();
		ImGui::SameLine();
		ImGui::DragFloat("##x", &value->x, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		//Y
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.7f, 0.3f, 1.0f));
		ImGui::PushFont(GEditorStyle->boldFont);
		if (ImGui::Button("Y", buttonSize))
			value->y = resetValue;

		ImGui::PopStyleColor(3);
		ImGui::PopFont();
		ImGui::SameLine();
		ImGui::DragFloat("##y", &value->y, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		//Z
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.35f, 0.9f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
		ImGui::PushFont(GEditorStyle->boldFont);
		if (ImGui::Button("Z", buttonSize))
			value->z = resetValue;

		ImGui::PopStyleColor(3);
		ImGui::PopFont();
		ImGui::SameLine();
		ImGui::DragFloat("##z", &value->z, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();	

		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();

		return true;
	}
}
