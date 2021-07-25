#include "ImguiUtil.h"

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
}
