#include "vehicle_tab.hpp"

#include "imgui.h"
#include <script/script.hpp>
#include <fiber_pool.hpp>
#include <rage/joaat.hpp>
#include <util/vehicle.hpp>
#include <util/entity.hpp>
#include <util/player_util.hpp>
#include <service/notification_service.hpp>
#include <service/player_service.hpp>
#include "../imgui/imgui_custom_widgets.hpp"
#include <util/color.hpp>
#include <gta/vehicles.hpp>

namespace Vengury {
	void VehicleTab::Render()
	{
		RenderTopbar();

		ImGuiWidgets::SeparatorText("General");

		ImGui::Checkbox("Godmode", &m_Godmode);
		
		ImGui::SameLine();

		ImGui::Checkbox("Honk Boosting", &m_HonkBoosting);

		ImGui::Checkbox("Rainbow Car", &m_RainbowCar);

		ImGui::SameLine();

		ImGui::Checkbox("Invisible", &m_Invisible);

		ImGuiWidgets::SeparatorText("Fun");
		
		if (!m_InVehicle)
			ImGui::BeginDisabled();

		if (ImGui::Button("Fling Vehicle Up")) {
			g_fiber_pool->QueueJob([](RagePlayer player, Ped ped, Vehicle vehicle) {
				if (!ENTITY::DOES_ENTITY_EXIST(vehicle)) {
					g_notification_service->PushNotification("You are not in a Vehicle.", 500, NotificationType::Subtitle);
					return;
				}

				auto velocity = ENTITY::GET_ENTITY_VELOCITY(vehicle);
				velocity.z += 20;

				entity::TakeControlOf(vehicle, 30);

				ENTITY::SET_ENTITY_VELOCITY(vehicle, velocity.x, velocity.y, velocity.z);
				g_notification_service->PushNotification("Flinged Vehicle upwards.", 500, NotificationType::Subtitle);
			});
		}

		ImGui::SameLine();

		if (ImGui::Button("Fling Vehicle Forward")) {
			g_fiber_pool->QueueJob([](RagePlayer player, Ped ped, Vehicle vehicle) {
				if (!ENTITY::DOES_ENTITY_EXIST(vehicle)) {
					g_notification_service->PushNotification("You are not in a Vehicle.", 500, NotificationType::Subtitle);
					return;
				}

				auto velocity = ENTITY::GET_ENTITY_VELOCITY(vehicle);
				auto forward = ENTITY::GET_ENTITY_FORWARD_VECTOR(vehicle);

				velocity.x += forward.x * 20;
				velocity.y += forward.y * 20;
				velocity.z += forward.z * 20;

				entity::TakeControlOf(vehicle, 30);
				ENTITY::SET_ENTITY_VELOCITY(vehicle, velocity.x, velocity.y, velocity.z);
				g_notification_service->PushNotification("Flinged Vehicle forward.", 500, NotificationType::Subtitle);
				});
		}

		if (!m_InVehicle)
			ImGui::EndDisabled();

		ImGuiWidgets::SeparatorText("Spawning");

		RenderSpawningMenu();
	}

	void VehicleTab::Tick()
	{
		const auto player = PLAYER::PLAYER_ID();

		Ped playerPed;

		if (!player::GetPlayerPed(player, &playerPed)) {
			m_InVehicle = false;
			return;
		}

		Vehicle vehicle;

		if (!vehicle::GetVehicle(playerPed, &vehicle)) {
			m_InVehicle = false;
			return;
		}

		m_InVehicle = true;

		// Godmode
		{
			ENTITY::SET_ENTITY_INVINCIBLE(vehicle, m_Godmode);
		}

		if (m_HonkBoosting) {
			if (AUDIO::IS_HORN_ACTIVE(vehicle)) {
				auto velocity = ENTITY::GET_ENTITY_VELOCITY(vehicle);
				auto forward = ENTITY::GET_ENTITY_FORWARD_VECTOR(vehicle);

				velocity.x += forward.x * 20;
				velocity.y += forward.y * 20;
				velocity.z += forward.z * 20;

				entity::TakeControlOf(vehicle, 30);
				ENTITY::SET_ENTITY_VELOCITY(vehicle, velocity.x, velocity.y, velocity.z);
			}
		}

		if (m_RainbowCar) {
			if (VEHICLE::GET_PED_IN_VEHICLE_SEAT(vehicle, -1, 0) == playerPed) {
				const auto duration = std::chrono::system_clock::now().time_since_epoch();
				const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

				float seconds = 4;
				float hue = (millis % (int)(seconds * 1000)) / (seconds * 1000);

				Color3 rgb = HSVtoRGB(hue, 1, 1);

				VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(vehicle, rgb.r * 255, rgb.g * 255, rgb.b * 255);
			}
		}

		{
			if (VEHICLE::GET_PED_IN_VEHICLE_SEAT(vehicle, -1, 0) == playerPed) {
				ENTITY::SET_ENTITY_VISIBLE(vehicle, !m_Invisible, 0);
				
				if (m_Invisible)
					NETWORK::SET_ENTITY_LOCALLY_INVISIBLE(vehicle);
				else
					NETWORK::SET_ENTITY_LOCALLY_VISIBLE(vehicle);
			}
		}
	}

	void VehicleTab::RenderTopbar() const
	{
		if (!m_InVehicle)
			ImGui::BeginDisabled();

		if (ImGui::Button("Repair Vehicle")) {
			g_fiber_pool->QueueJob([](RagePlayer player, Ped ped, Vehicle vehicle) {
				if (!ENTITY::DOES_ENTITY_EXIST(vehicle)) {
					g_notification_service->PushNotification("You are not in a Vehicle.", 1500, NotificationType::Subtitle);
					return;
				}

				vehicle::RepairVehicle(vehicle);
				g_notification_service->PushNotification("Repaired vehicle.", 1500, NotificationType::Subtitle);
				});
		}

		ImGui::SameLine();

		if (ImGui::Button("Full Upgrade")) {
			g_fiber_pool->QueueJob([](RagePlayer player, Ped ped, Vehicle vehicle) {
				if (!ENTITY::DOES_ENTITY_EXIST(vehicle)) {
					g_notification_service->PushNotification("You are not in a Vehicle.", 1500, NotificationType::Subtitle);
					return;
				}

				vehicle::MaxUpgradeVehicle(vehicle);
				g_notification_service->PushNotification("Fully upgraded Vehicle.", 1500, NotificationType::Subtitle);
				});
		}

		ImGui::SameLine();

		if (ImGui::Button("Delete Vehicle")) {
			g_fiber_pool->QueueJob([](RagePlayer player, Ped ped, Vehicle vehicle) {
				if (!ENTITY::DOES_ENTITY_EXIST(vehicle)) {
					g_notification_service->PushNotification("You are not in a Vehicle.", 1500, NotificationType::Subtitle);
					return;
				}

				if (entity::DeleteEntity(vehicle))
					g_notification_service->PushNotification("Deleted vehicle.", 1500, NotificationType::Subtitle);
				else
					g_notification_service->PushNotification("Failed to delete vehicle.", 1500, NotificationType::Subtitle);
			});
		}

		if (!m_InVehicle)
			ImGui::EndDisabled();
	}

	void VehicleTab::RenderSpawningMenu()
	{
		ImGui::Checkbox("God Mode##spawning", &m_SpawningGodMode);

		ImGui::SameLine();

		ImGui::Checkbox("Invisible ##spawning", &m_SpawningInvisible);

		ImGui::InputText("##spawninginput", m_SpawningInput, IM_ARRAYSIZE(m_SpawningInput), 0, 0);

		ImGui::SameLine();

		if (ImGui::Button("Spawn")) {
			g_fiber_pool->QueueJob([&](RagePlayer player, Ped ped, Vehicle vehicle) {
				const auto input = std::string(m_SpawningInput);
				const auto hash = rage::joaat(input);

				if (!STREAMING::IS_MODEL_VALID(hash)) {
					g_notification_service->PushNotification("~r~Invalid vehicle model name.", 1500, NotificationType::Subtitle);
					return;
				}

				auto pos = ENTITY::GET_ENTITY_COORDS(ped, true);
				
				SpawnVehicle(hash, pos);

				g_notification_service->PushNotification("Spawned vehicle.", 1500, NotificationType::Subtitle);
				});
		}
	}

	void VehicleTab::SpawnVehicle(Hash hash, Vector3 pos)
	{
		auto veh = vehicle::Spawn(hash, pos, 0.f, true, false);

		if (m_SpawningGodMode)
			ENTITY::SET_ENTITY_INVINCIBLE(veh, true);

		ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&veh);
	}
}