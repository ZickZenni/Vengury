#include "self_tab.hpp"

#include "imgui.h"
#include <util/player_util.hpp>
#include <script/script.hpp>
#include <fiber_pool.hpp>
#include <rage/joaat.hpp>
#include <util/vehicle.hpp>
#include <service/player_service.hpp>
#include <service/notification_service.hpp>
#include <service/object_service.hpp>
#include <util/entity.hpp>
#include <gui/imgui/imgui_custom_widgets.hpp>

namespace Vengury {
	void SelfTab::Render()
	{
		RenderTopbar();

		ImGuiWidgets::SeparatorText("General");

		if (ImGui::Checkbox("God Mode", &m_GodMode)) {
			if (!m_GodMode) {
				g_fiber_pool->QueueJob([](RagePlayer player, Ped ped, Vehicle vehicle) {
					ENTITY::SET_ENTITY_INVINCIBLE(ped, false);
				});
			}
		}

		ImGui::SameLine();

		ImGui::PushItemWidth(120);

		ImGui::Checkbox("Super Jump##self", &m_SuperJump);

		ImGui::Checkbox("Fast Run##self", &m_FastRun);

		ImGui::SameLine();

		ImGui::Checkbox("Never Wanted##self", &m_NeverWanted);

		ImGui::Checkbox("No Ragdoll##self", &m_NoRagdoll);

		ImGui::PopItemWidth();

		ImGuiWidgets::SeparatorText("Fun");

		if (ImGui::Button("Explode")) {
			g_fiber_pool->QueueJob([](RagePlayer player, Ped ped, Vehicle vehicle) {
				const auto targetPos = ENTITY::GET_ENTITY_COORDS(ped, false);

				FIRE::ADD_EXPLOSION(targetPos.x, targetPos.y, targetPos.z, eExplosionTag::STICKYBOMB, 5, true, false, 2, false);
				});
		}

		ImGui::SameLine();

		if (ImGui::Button("Set Player Drunk")) {
			g_fiber_pool->QueueJob([](RagePlayer player, Ped ped, Vehicle vehicle) {
				if (!STREAMING::HAS_ANIM_SET_LOADED("MOVE_M@DRUNK@VERYDRUNK")) {
					STREAMING::REQUEST_ANIM_SET("MOVE_M@DRUNK@VERYDRUNK");
					while (!STREAMING::HAS_ANIM_SET_LOADED("MOVE_M@DRUNK@VERYDRUNK"))
						Script::GetCurrent()->YieldScript();
				}

				PED::SET_PED_MOVEMENT_CLIPSET(ped, "MOVE_M@DRUNK@VERYDRUNK", 1.0f);
				AUDIO::SET_PED_IS_DRUNK(PLAYER::PLAYER_PED_ID(), true);
				PED::SET_PED_CONFIG_FLAG(ped, 100, true);
				CAM::SHAKE_GAMEPLAY_CAM("DRUNK_SHAKE", 1.0f);
				g_notification_service->PushNotification("Set player drunk.", 500, NotificationType::Subtitle);
			});
		}

		ImGui::SameLine();

		if (ImGui::Button("Remove Player Drunk")) {
			g_fiber_pool->QueueJob([](RagePlayer player, Ped ped, Vehicle vehicle) {
				PED::RESET_PED_MOVEMENT_CLIPSET(ped, 0.0);
				AUDIO::SET_PED_IS_DRUNK(PLAYER::PLAYER_PED_ID(), true);
				PED::SET_PED_CONFIG_FLAG(ped, 100, true);
				CAM::STOP_GAMEPLAY_CAM_SHAKING(true);
				g_notification_service->PushNotification("Removed player drunk.", 500, NotificationType::Subtitle);
			});
		}


		/*
		if (ImGui::Button("Attach Burger")) {
			QUEUE_JOB_BEGIN_CLAUSE() {
				Ped targetPed;
				const auto burger = g_object_service->Create(rage::joaat("prop_cs_burger_01"), 0, 0, 0, false);

				if (!burger) {
					g_notification_service->PushNotification("~r~Failed to create Burger.", 500, NotificationType::Subtitle);
					return;
				}

				entity::AttachEntityToEntity(burger, targetPed, PED::GET_PED_BONE_INDEX(targetPed, 0x796e), 0, 0, 0.3f, 0, 0, 0, false, false, true, 0, false);
				g_notification_service->PushNotification("~g~Attached Burger.", 500, NotificationType::Subtitle);
			}
			QUEUE_JOB_END_CLAUSE
		}*/
	}

	void SelfTab::Tick()
	{
		const auto player = PLAYER::PLAYER_ID();

		Ped playerPed = 0;

		if (!player::GetPlayerPed(player, &playerPed))
			return;

		// Godmode
		if (m_GodMode) {
			ENTITY::SET_ENTITY_INVINCIBLE(playerPed, true);
		}

		// Never Wanted
		if (m_NeverWanted) {
			PLAYER::SET_PLAYER_WANTED_LEVEL(player, 0, false);
			PLAYER::SET_PLAYER_WANTED_LEVEL_NOW(player, false);
		}

		// Super Jump
		if (m_SuperJump) {
			MISC::SET_SUPER_JUMP_THIS_FRAME(player);
		}

		// Fast Run
		{
			PLAYER::SET_RUN_SPRINT_MULTIPLIER_FOR_PLAYER(player, m_FastRun ? 1.49f : 1.0f);
		}

		{
			PED::SET_PED_CAN_RAGDOLL(player, m_NoRagdoll);
		}
	}

	void SelfTab::RenderTopbar()
	{
		if (ImGui::Button("Suicide")) {
			g_fiber_pool->QueueJob([](RagePlayer player, Ped ped, Vehicle vehicle) {
				ENTITY::SET_ENTITY_HEALTH(ped, 0, 0, 0);
				g_notification_service->PushNotification("Suicided player.", 500, NotificationType::Subtitle);
			});
		}

		ImGui::SameLine();

		if (ImGui::Button("Heal")) {
			g_fiber_pool->QueueJob([](RagePlayer player, Ped ped, Vehicle vehicle) {
				ENTITY::SET_ENTITY_HEALTH(ped, ENTITY::GET_ENTITY_MAX_HEALTH(ped), 0, 0);
				g_notification_service->PushNotification("Healed player.", 500, NotificationType::Subtitle);
			});
		}

		ImGui::SameLine();

		if (ImGui::Button("Fill Armor")) {
			g_fiber_pool->QueueJob([](RagePlayer player, Ped ped, Vehicle vehicle) {
				PED::SET_PED_ARMOUR(ped, 100);
				g_notification_service->PushNotification("Filled armor.", 500, NotificationType::Subtitle);
			});
		}

		ImGui::SameLine();

		if (ImGui::Button("Clear Wanted Level")) {
			g_fiber_pool->QueueJob([](RagePlayer player, Ped ped, Vehicle vehicle) {
				PLAYER::SET_PLAYER_WANTED_LEVEL(player, 0, false);
				PLAYER::SET_PLAYER_WANTED_LEVEL_NOW(player, false);
				g_notification_service->PushNotification("Cleared wanted level.", 500, NotificationType::Subtitle);
			});
		}
	}
}
