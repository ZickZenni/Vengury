#include "teleport_tab.hpp"

#include <fiber_pool.hpp>
#include <script/script.hpp>
#include <util/entity.hpp>
#include <util/vehicle.hpp>
#include <service/notification_service.hpp>

namespace Vengury {
	void TeleportTab::Render()
	{
		if (ImGui::Button("Teleport to Waypoint")) {
			g_fiber_pool->QueueJob([](RagePlayer player, Ped ped, Vehicle vehicle) {
				TeleportToBlip(ped, HUD::GET_WAYPOINT_BLIP_ENUM_ID());
			});
		}

		ImGui::SameLine();

		if (ImGui::Button("Teleport to Personal Vehicle")) {
			g_fiber_pool->QueueJob([](RagePlayer player, Ped ped, Vehicle vehicle) {
				TeleportToBlip(ped, 225);
			});
		}

		ImGui::SameLine();

		if (ImGui::Button("Teleport to mission")) {
			g_fiber_pool->QueueJob([](RagePlayer player, Ped ped, Vehicle vehicle) {
				TeleportToBlip(ped, 271);
			});
		}


		ImGui::SameLine();

		if (ImGui::Button("Teleport forward")) {
			g_fiber_pool->QueueJob([](RagePlayer player, Ped ped, Vehicle vehicle) {
				auto position = ENTITY::GET_ENTITY_COORDS(ped, false);
				auto targetForward = ENTITY::GET_ENTITY_FORWARD_VECTOR(ped);
				position.x += targetForward.x * 2;
				position.y += targetForward.y * 2;
				position.z += targetForward.z * 2;

				if (entity::TeleportEntityToCoords(ped, position, false, true))
					g_notification_service->PushNotification("Teleported forward.", 600, NotificationType::Subtitle);
				else
					g_notification_service->PushNotification("Failed to teleport forward.", 1500, NotificationType::Subtitle);
			});
		}
	}

	void TeleportTab::TeleportToBlip(Ped ped, int blipId)
	{
		const auto blip = HUD::GET_FIRST_BLIP_INFO_ID(blipId);

		if (!HUD::IS_BLIP_ON_MINIMAP(blip))
			return;

		const auto position = HUD::GET_BLIP_INFO_ID_COORD(blip);

		if (entity::TeleportEntityToCoords(ped, position, true, true))
			g_notification_service->PushNotification("Teleported to waypoint.", 600, NotificationType::Subtitle);
		else
			g_notification_service->PushNotification("Failed to teleport to waypoint.", 1500, NotificationType::Subtitle);
	}
}