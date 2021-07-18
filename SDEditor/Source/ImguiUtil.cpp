#include "ImguiUtil.h"

namespace ImGui
{
	bool DragXMFloat3(const char* label, XMFLOAT3* v, float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
	{
		return DragScalarN(label, ImGuiDataType_Float, v, 3, v_speed, &v_min, &v_max, format, flags);
	}
}
