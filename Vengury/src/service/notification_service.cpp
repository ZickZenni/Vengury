#include "notification_service.hpp"

#include <script/script.hpp>
#include <fiber_pool.hpp>
#include <gta/natives.hpp>

namespace Vengury {
	NotificationService::NotificationService()
	{
		g_notification_service = this;
	}

	NotificationService::~NotificationService()
	{
		g_notification_service = nullptr;
	}

	void NotificationService::RunSubtitle()
	{
		while (true)
		{
			const auto service = g_notification_service;
			if (service->m_subtitle_notifications.size() > 0) {
				const auto& notification = service->m_subtitle_notifications[0];

				HUD::BEGIN_TEXT_COMMAND_PRINT("STRING");
				HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(notification.message);
				HUD::END_TEXT_COMMAND_PRINT(notification.duration, true);

				service->m_subtitle_notifications.erase(service->m_subtitle_notifications.begin());

				Script::GetCurrent()->YieldScript(std::chrono::milliseconds(notification.duration));
			}

			Script::GetCurrent()->YieldScript();
		}
	}

	void NotificationService::RunMinimap()
	{
		while (true)
		{
			const auto service = g_notification_service;
			if (service->m_minimap_notifications.size() > 0) {
				const auto& notification = service->m_minimap_notifications[0];
				
				HUD::BEGIN_TEXT_COMMAND_THEFEED_POST("STRING");
				HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(notification.message);
				HUD::END_TEXT_COMMAND_THEFEED_POST_TICKER(false, notification.duration);

				service->m_minimap_notifications.erase(service->m_minimap_notifications.begin());

				Script::GetCurrent()->YieldScript(std::chrono::milliseconds(notification.duration));
			}
			Script::GetCurrent()->YieldScript();
		}
	}

	void NotificationService::PushNotification(const char* message, int duration, NotificationType type)
	{
		Notification notification{
			message, duration
		};

		switch (type) {
			case Subtitle: m_subtitle_notifications.push_back(notification); break;
			case Minimap: m_minimap_notifications.push_back(notification); break;
		}
	}

	void NotificationService::Push(const char* title, std::string message, int duration)
	{
		MenuNotification notification{
			title, std::move(message), duration
		};
		m_menu_notifications.push_back(notification);
	}
}
