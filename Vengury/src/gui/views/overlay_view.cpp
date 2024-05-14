#include "../views.hpp"
#include "util/gta_util.hpp"

namespace Vengury {
	void Views::RenderOverlay() {
		if (!*g_settings.options["overlay_enabled"].get<bool*>())
			return;

		ImGui::SetNextWindowPos(ImVec2(20, 20));
		ImGui::SetNextWindowBgAlpha(0.795f);

		if (ImGui::Begin("InformationPanel", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoMouseInputs)) {
			ImGui::Text(std::format("{:.0f} fps", ImGui::GetIO().Framerate).c_str());

			if (CNetworkPlayerMgr* network_player_mgr = gta_util::GetNetworkPlayerManager())
			{
				ImGui::Text(std::format("Players: {} / {}", network_player_mgr->m_player_count, network_player_mgr->m_player_limit).c_str());
			}
		}
		ImGui::End();
	}
}