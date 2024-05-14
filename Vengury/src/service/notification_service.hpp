#pragma once

namespace Vengury {
	class Views;

	enum NotificationType {
		Subtitle,
		Minimap
	};

	struct Notification {
		const char* message;
		int duration;
	};

	struct MenuNotification {
		const char* title;
		std::string message;
		int duration;
	};

	class NotificationService {
	public:
		NotificationService();
		~NotificationService();

		static void RunSubtitle();
		static void RunMinimap();

		void PushNotification(const char* message, int duration, NotificationType type);

		void Push(const char* title, std::string message, int duration);

	private:
		std::vector<MenuNotification> m_menu_notifications{};
		std::vector<Notification> m_subtitle_notifications{};
		std::vector<Notification> m_minimap_notifications{};

		friend Views;
	};

	inline NotificationService* g_notification_service{};
}