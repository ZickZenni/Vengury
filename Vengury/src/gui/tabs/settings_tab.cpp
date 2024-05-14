#include "settings_tab.h"

#include "common.hpp"
#include "imgui.h"
#include <gui/imgui/imgui_custom_widgets.hpp>

namespace Vengury
{
	void SettingsTab::Render()
	{
		if (ImGui::Button("Unload"))
		{
			g_running = false;
		}

		ImGuiWidgets::SeparatorText("General");

		if (ImGui::Checkbox("Show Overlay", g_settings.options["overlay_enabled"].get<bool*>()))
			g_settings.Save();
	}
}