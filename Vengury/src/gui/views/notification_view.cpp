#include "../views.hpp"
#include "util/gta_util.hpp"
#include <service/notification_service.hpp>
#include <util/time.hpp>

namespace Vengury {
	MenuNotification* m_CurrentNotify = NULL;
	int64_t m_StartTime = 0;

	void Views::RenderNotification() {
		const auto displaySize = ImGui::GetMainViewport()->Size;

		if (m_CurrentNotify && Time::GetMillis() >= (m_StartTime + m_CurrentNotify->duration)) {
			m_CurrentNotify = nullptr;
			g_notification_service->m_menu_notifications.erase(g_notification_service->m_menu_notifications.begin());
		}

		if (!m_CurrentNotify && g_notification_service->m_menu_notifications.size() > 0) {
			m_StartTime = Time::GetMillis();
			m_CurrentNotify = &g_notification_service->m_menu_notifications[0];
		}

		if (m_CurrentNotify) {
			ImGui::SetNextWindowPos(ImVec2(displaySize.x - 470, 20));
			ImGui::SetNextWindowBgAlpha(0.795f);
			ImGui::SetNextWindowSize(ImVec2(450, 250));

			if (ImGui::Begin("Notification", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoMouseInputs)) {
				ImGui::Text(m_CurrentNotify->title);
				ImGui::Text(m_CurrentNotify->message.c_str());
			}
			ImGui::End();
		}
	}
}