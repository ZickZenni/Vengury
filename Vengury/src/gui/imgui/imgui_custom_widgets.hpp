#pragma once

#include <imgui_internal.h>

namespace ImGuiWidgets {
	void SeparatorText(const char* label);
	void SeparatorTextEx(ImGuiID id, const char* label, const char* label_end, float extra_w);

	template<typename T> T ImMax(T lhs, T rhs) { return lhs >= rhs ? lhs : rhs; }
	ImVec2 ImMax(const ImVec2& lhs, const ImVec2& rhs);

	float  ImTrunc(float f);
	ImVec2 ImTrunc(const ImVec2& v);
}