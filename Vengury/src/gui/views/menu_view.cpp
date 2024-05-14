#include "../views.hpp"
#include <gui/gui.hpp>

namespace Vengury {
	void Views::RenderMenu() {
		if (!g_gui.m_opened)
			return;

		if (ImGui::Begin("Vengury", nullptr, ImGuiWindowFlags_NoCollapse))
		{
			if (ImGui::BeginChild("#tabbuttons", ImVec2(148, 0.f), 0, ImGuiWindowFlags_NoDecoration)) {
				ImGui::SameLine(8.f);

				if (ImGui::BeginTable("#tabbuttonstable", 1, ImGuiTableFlags_None, ImVec2(140, 48))) {
					int i = 0;
					for (const auto& tab : g_gui.m_tabs) {
						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						ImGui::PushID(tab.name);

						bool a = g_gui.m_currentTab == i;
						if (!a) {
							ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
							ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 0, 0, 0));
						}
						else {
							ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(39, 39, 255, 70));
						}
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.f);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.f);

						if (ImGui::Button(tab.name, ImVec2(128, 64))) {
							g_gui.m_currentTab = i;
						}

						ImGui::PopStyleVar(2);
						ImGui::PopStyleColor(a ? 1 : 2);
						ImGui::PopID();
						i++;
					}

					ImGui::EndTable();
				}

			}
			ImGui::EndChild();

			ImGui::SameLine();

			// Tab Content
			float width = ImGui::GetWindowSize().x;

			if (ImGui::BeginChild("tabcontent", ImVec2(0, 0), 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration)) {
				auto it = g_gui.m_tabs.begin();
				std::advance(it, g_gui.m_currentTab);

				const auto x = ImGui::GetCursorPosX();

				auto windowWidth = ImGui::GetWindowSize().x;
				auto textWidth = ImGui::CalcTextSize(it->name).x;

				ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
				ImGui::Text(it->name);
				ImGui::SetCursorPosX(x);

				if (ImGui::BeginChild("tabcontent", ImVec2(0, 0), 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration)) {
					if (it->tab)
						it->tab->Render();
				}

				ImGui::EndChild();
			}

			ImGui::EndChild();
		}
		ImGui::End();
	}
}