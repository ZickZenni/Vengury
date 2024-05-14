#include "session_tab.hpp"
#include <pointers.hpp>
#include <util/gta_util.hpp>
#include <gui/imgui/imgui_custom_widgets.hpp>
#include <service/player_service.hpp>
#include <service/notification_service.hpp>
#include <script/script.hpp>
#include <fiber_pool.hpp>
#include <util/player_util.hpp>
#include <util/vehicle.hpp>
#include <util/entity.hpp>
#include <network/snSession.hpp>
#include <gta/net_game_event.hpp>
#include <net/packet.hpp>
#include <math.hpp>

namespace Vengury {
	void SessionTab::Render()
	{
		ImGui::Text("In Session: %s", g_player_service->IsConnected() ? "Yes" : "No");

		if (!g_player_service->IsConnected())
			return;

		const auto networkManager = gta_util::GetNetworkPlayerManager();

		if (!networkManager || networkManager->m_player_count == 0)
			return;

		ImGui::Text("Player Count: %d / %d", networkManager->m_player_count, networkManager->m_player_limit);

		ImGuiWidgets::SeparatorText("Players");

		RenderPlayerList(networkManager);

		ImGui::SameLine();

		if (g_player_service->m_selected_peer)
			RenderSelectedPlayer();
	}

	void SessionTab::Tick()
	{
		if (!g_player_service->IsConnected()) {
			g_player_service->m_selected_peer = 0;
			return;
		}

		const auto networkManager = gta_util::GetNetworkPlayerManager();

		if (!networkManager || networkManager->m_player_count == 0) {
			g_player_service->m_selected_peer = 0;
			return;
		}

		TickSelectedPeer(networkManager);
	}

	void SessionTab::TickSelectedPeer(CNetworkPlayerMgr* mgr)
	{
		for (uint16_t i = 0; i < mgr->m_player_limit; ++i) {
			const auto player = mgr->m_player_list[i];

			if (!player)
				continue;

			const auto netData = player->get_net_data();

			if (!netData)
				continue;

			if (netData->m_peer_id == g_player_service->m_selected_peer) {
				// Ticking
				Ped targetPed;

				if (!player::GetPlayerPed(g_player_service->GetIdFromPeerId(g_player_service->m_selected_peer), &targetPed)) {
					m_InVehicle = false;
					return;
				}

				m_InVehicle = PED::IS_PED_IN_ANY_VEHICLE(targetPed, true);
				return;
			}
		}

		m_InVehicle = false;
		g_player_service->m_selected_peer = 0;
	}

	void SessionTab::RenderPlayerList(CNetworkPlayerMgr* mgr)
	{
		if (ImGui::BeginChild("#playerList", ImVec2(148, 0.f), 0, ImGuiWindowFlags_NoDecoration)) {

			if (ImGui::BeginTable("#playerListTable", 1, ImGuiTableFlags_None, ImVec2(148, 37))) {
				const auto localPlayer = mgr->m_local_net_player;

				if (!localPlayer || !localPlayer->get_net_data()) {
					ImGui::EndTable();
					return;
				}

				for (uint16_t i = 0; i < mgr->m_player_limit; ++i) {
					const auto player = mgr->m_player_list[i];

					if (!player)
						continue;

					const auto netData = player->get_net_data();

					if (!netData)
						continue;

					// Ignore local player
					if (netData->m_peer_id == localPlayer->get_net_data()->m_peer_id)
						continue;

					ImGui::TableNextRow();
					ImGui::TableNextColumn();

					const auto isSelected = g_player_service->m_selected_peer == netData->m_peer_id;

					if (isSelected)
						ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(39, 39, 255, 70));

					if (ImGui::Button(player->get_name(), ImVec2(148, 37))) {
						g_player_service->m_selected_peer = netData->m_peer_id;
					}

					if (isSelected)
						ImGui::PopStyleColor();
				}
				ImGui::EndTable();
			}
		}
		ImGui::EndChild();
	}

	void SessionTab::RenderSelectedPlayer()
	{
		if (ImGui::BeginChild("#selectedplayer", ImVec2(0.f, 0.f), 0, ImGuiWindowFlags_NoDecoration)) {
			const auto player = g_player_service->GetPlayerByPeerId(g_player_service->m_selected_peer);

			if (!player) {
				ImGui::EndChild();
				return;
			}

			const auto netData = player->GetNetData();
			const auto netPlayer = player->GetNetPlayer();

			if (!netData) {
				ImGui::EndChild();
				return;
			}

			ImGui::Text("Name: %s", netPlayer->get_name());
			ImGui::Text("Player Id: %d", netPlayer->m_player_id);

			if (!netData) {
				ImGui::EndChild();
				return;
			}

			ImGui::Text("R* Id: %d", netData->m_gamer_handle.m_rockstar_id);
			ImGui::Text("Is Cheater: %s", netPlayer->m_is_cheater ? "Yes" : "No");
			ImGui::Text("Is R* Dev: %s", netPlayer->m_is_rockstar_dev ? "Yes" : "No");
			ImGui::Text("Is R* Admin: %s", netPlayer->m_is_rockstar_dev ? "Yes" : "No");
			ImGui::Text("Is Spectating: %s", netPlayer->m_is_spectating ? "Yes" : "No");

			if (ImGui::BeginChild("#teleport", ImVec2(200.f, 150.f), 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground)) {
				ImGuiWidgets::SeparatorText("Teleport");

				if (ImGui::Button("Teleport to Player")) {
					g_fiber_pool->QueueJob([](RagePlayer player, Ped ped, Vehicle vehicle) {
						Ped targetPed;

						if (!player::GetPlayerPed(g_player_service->GetIdFromPeerId(g_player_service->m_selected_peer), &targetPed))
							return;

						const auto targetPos = ENTITY::GET_ENTITY_COORDS(targetPed, false);
						entity::TeleportEntityToCoords(ped, targetPos, false, true);
						g_notification_service->PushNotification("Teleported to player.", 1500, NotificationType::Subtitle);
					});
				}

				if (!m_InVehicle)
					ImGui::BeginDisabled();

				if (ImGui::Button("Teleport into Vehicle")) {
					g_fiber_pool->QueueJob([](RagePlayer player, Ped ped, Vehicle localVehicle) {
						Ped targetPed;

						if (!player::GetPlayerPed(g_player_service->GetIdFromPeerId(g_player_service->m_selected_peer), &targetPed))
							return;

						Vehicle vehicle;

						if (!vehicle::GetVehicle(targetPed, &vehicle) || !VEHICLE::ARE_ANY_VEHICLE_SEATS_FREE(vehicle)) {
							g_notification_service->PushNotification("Vehicle is full.", 1500, NotificationType::Subtitle);
							return;
						}

						for (int i = -1; i < 2; i++) {
							if (VEHICLE::IS_VEHICLE_SEAT_FREE(vehicle, i, false)) {
								const auto targetPos = ENTITY::GET_ENTITY_COORDS(vehicle, false);
								entity::TeleportEntityToCoords(ped, targetPos, false, false);
								PED::SET_PED_INTO_VEHICLE(ped, vehicle, i);
								g_notification_service->PushNotification("Teleported into vehicle.", 1500, NotificationType::Subtitle);
								break;
							}
						}

						g_notification_service->PushNotification("Vehicle is full.", 1500, NotificationType::Subtitle);
					});
				}

				if (ImGui::Button("Bring Vehicle to me")) {
					g_fiber_pool->QueueJob([](RagePlayer player, Ped ped, Vehicle localVehicle) {
						Ped targetPed;

						if (!player::GetPlayerPed(g_player_service->GetIdFromPeerId(g_player_service->m_selected_peer), &targetPed))
							return;

						Vehicle vehicle;

						if (!vehicle::GetVehicle(targetPed, &vehicle)) {
							g_notification_service->PushNotification("Player is not in a Vehicle.", 1500, NotificationType::Subtitle);
							return;
						}

						const auto targetPos = ENTITY::GET_ENTITY_COORDS(ped, false);
						entity::TeleportEntityToCoords(vehicle, targetPos, false, true);

						g_notification_service->PushNotification("Brought vehicle to me.", 1500, NotificationType::Subtitle);
					});
				}

				if (!m_InVehicle)
					ImGui::EndDisabled();
			}

			ImGui::EndChild();

			ImGui::SameLine();

			if (ImGui::BeginChild("#griefing", ImVec2(400.f, 100.f), 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground)) {
				ImGuiWidgets::SeparatorText("Griefing");

				if (ImGui::Button("Explode Player")) {
					g_fiber_pool->QueueJob([](RagePlayer player, Ped ped, Vehicle vehicle) {
						Ped targetPed;

						if (!player::GetPlayerPed(g_player_service->GetIdFromPeerId(g_player_service->m_selected_peer), &targetPed))
							return;

						const auto targetPos = ENTITY::GET_ENTITY_COORDS(targetPed, false);

						FIRE::ADD_EXPLOSION(targetPos.x, targetPos.y, targetPos.z, eExplosionTag::STICKYBOMB, 5, true, false, 2, false);
						g_notification_service->PushNotification("Exploded player.", 1500, NotificationType::Subtitle);
					});
				}

				if (ImGui::Button("Kick Player")) {
					g_fiber_pool->QueueJob([](RagePlayer localPlayer, Ped ped, Vehicle vehicle) {
						const auto player = g_player_service->GetPlayerByPeerId(g_player_service->m_selected_peer);

						if (!player)
							return;

						const auto sessionPlayer = player->GetSessionPlayer();

						if (!sessionPlayer)
							return;

						Packet msg{};

						msg.write_message(rage::eNetMessage::MsgRadioStationSyncRequest);
						auto msg_id = sessionPlayer->m_msg_id;

						for (int j = 0; j < 2000; j++)
						{
							msg.Send(msg_id);
						}

						/*rage::snMsgRemoveGamersFromSessionCmd cmd{};
						cmd.m_session_id = gta_util::GetNetwork()->m_game_session_ptr->m_rline_session.m_session_id;
						cmd.m_num_peers = 1;
						cmd.m_handles[0] = player->GetNetData()->m_gamer_handle;
						cmd.m_unk = 19;

						g_pointers->m_handle_remove_gamer_cmd(gta_util::GetNetwork()->m_game_session_ptr, sessionPlayer, &cmd);*/

						g_notification_service->PushNotification("Kicked player.", 1500, NotificationType::Subtitle);
					});
				}
			}

			ImGui::EndChild();

			ImGuiWidgets::SeparatorText("Trolling");

			if (ImGui::Button("Drop Tank onto Player")) {
				g_fiber_pool->QueueJob([](RagePlayer player, Ped ped, Vehicle vehicle) {
					Ped targetPed;

					if (!player::GetPlayerPed(g_player_service->GetIdFromPeerId(g_player_service->m_selected_peer), &targetPed))
						return;

					auto targetPos = ENTITY::GET_ENTITY_COORDS(targetPed, false);
					targetPos.z += 12.f;

					constexpr auto hash = rage::joaat("rhino");
					auto veh = vehicle::Spawn(hash, targetPos, 0.f, true, false);

					ENTITY::SET_ENTITY_VELOCITY(veh, 0, 0, -205.f);
					ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&veh);
					VEHICLE::SET_VEHICLE_DOORS_LOCKED_FOR_ALL_PLAYERS(veh, true);
					g_notification_service->PushNotification("Dropped Tank onto player.", 1500, NotificationType::Subtitle);

					//script::get_current()->yield(std::chrono::milliseconds(3000));

					//entity::TakeControlOf(veh, 30);
					//ENTITY::SET_ENTITY_AS_MISSION_ENTITY(veh, true, true);
					//ENTITY::DELETE_ENTITY(&veh);
					//ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&veh);
				});
			}

			ImGui::SameLine();

			if (ImGui::Button("Place Tank in front")) {
				g_fiber_pool->QueueJob([](RagePlayer player, Ped ped, Vehicle vehicle) {
					Ped targetPed;

					if (!player::GetPlayerPed(g_player_service->GetIdFromPeerId(g_player_service->m_selected_peer), &targetPed))
						return;

					auto targetPos = ENTITY::GET_ENTITY_COORDS(targetPed, false);
					auto targetForward = ENTITY::GET_ENTITY_FORWARD_VECTOR(targetPed);
					targetPos.x += targetForward.x * 12;
					targetPos.y += targetForward.y * 12;
					targetPos.z += targetForward.z * 12;

					constexpr auto hash = rage::joaat("rhino");
					auto veh = vehicle::Spawn(hash, targetPos, 0.f, true, false);

					ENTITY::SET_ENTITY_VELOCITY(veh, -targetForward.x, -targetForward.y, -targetForward.z);
					ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&veh);
					VEHICLE::SET_VEHICLE_DOORS_LOCKED_FOR_ALL_PLAYERS(veh, true);
					g_notification_service->PushNotification("Placed Tank in front of player.", 1500, NotificationType::Subtitle);
				});
			}

			if (!m_InVehicle)
				ImGui::BeginDisabled();

			if (ImGui::Button("Stop Vehicle")) {
				g_fiber_pool->QueueJob([](RagePlayer player, Ped ped, Vehicle localVehicle) {
					Ped targetPed;

					if (!player::GetPlayerPed(g_player_service->GetIdFromPeerId(g_player_service->m_selected_peer), &targetPed))
						return;

					Vehicle vehicle;

					if (!vehicle::GetVehicle(targetPed, &vehicle))
						return;

					ENTITY::SET_ENTITY_VELOCITY(vehicle, 0, 0, 0);
					g_notification_service->PushNotification("Stopped vehicle of player.", 1500, NotificationType::Subtitle);
				});
			}

			ImGui::SameLine();

			if (ImGui::Button("Dismantle Vehicle")) {
				g_fiber_pool->QueueJob([](RagePlayer player, Ped ped, Vehicle localVehicle) {
					Ped targetPed;

					if (!player::GetPlayerPed(g_player_service->GetIdFromPeerId(g_player_service->m_selected_peer), &targetPed))
						return;

					Vehicle vehicle;

					if (!vehicle::GetVehicle(targetPed, &vehicle))
						return;

					const auto doors = VEHICLE::GET_NUMBER_OF_VEHICLE_DOORS(vehicle);
					auto i = 0;

					entity::TakeControlOf(vehicle, 30);
					while (i < doors) {
						VEHICLE::SET_VEHICLE_DOOR_BROKEN(vehicle, i, false);
						i++;
					}

					g_notification_service->PushNotification("Dismantled vehicle of player.", 1500, NotificationType::Subtitle);
				});
			}

			ImGui::SameLine();

			if (ImGui::Button("Delete Vehicle")) {
				g_fiber_pool->QueueJob([](RagePlayer player, Ped ped, Vehicle localVehicle) {
					Ped targetPed;

					if (!player::GetPlayerPed(g_player_service->GetIdFromPeerId(g_player_service->m_selected_peer), &targetPed))
						return;

					Vehicle vehicle;

					if (!vehicle::GetVehicle(targetPed, &vehicle))
						return;

					if (entity::DeleteEntity(vehicle))
						g_notification_service->PushNotification("Deleted vehicle from player.", 1500, NotificationType::Subtitle);
					else
						g_notification_service->PushNotification("Failed to delete vehicle from player.", 1500, NotificationType::Subtitle);
				});
			}

			ImGui::SameLine();

			if (ImGui::Button("Repair Vehicle")) {
				g_fiber_pool->QueueJob([](RagePlayer player, Ped ped, Vehicle localVehicle) {
					Ped targetPed;

					if (!player::GetPlayerPed(g_player_service->GetIdFromPeerId(g_player_service->m_selected_peer), &targetPed))
						return;

					Vehicle vehicle;

					if (!vehicle::GetVehicle(targetPed, &vehicle))
						return;

					vehicle::RepairVehicle(vehicle);
					g_notification_service->PushNotification("Repaired vehicle from player.", 1500, NotificationType::Subtitle);
				});
			}

			ImGui::SameLine();

			if (ImGui::Button("Fling Vehicle Forward")) {
				g_fiber_pool->QueueJob([](RagePlayer player, Ped ped, Vehicle localVehicle) {
					Ped targetPed;

					if (!player::GetPlayerPed(g_player_service->GetIdFromPeerId(g_player_service->m_selected_peer), &targetPed))
						return;

					Vehicle vehicle;

					if (!vehicle::GetVehicle(targetPed, &vehicle))
						return;

					auto velocity = ENTITY::GET_ENTITY_VELOCITY(vehicle);
					auto forward = ENTITY::GET_ENTITY_FORWARD_VECTOR(vehicle);

					velocity.x += forward.x * 20;
					velocity.y += forward.y * 20;
					velocity.z += forward.z * 20;

					entity::TakeControlOf(vehicle, 30);
					ENTITY::SET_ENTITY_VELOCITY(vehicle, velocity.x, velocity.y, velocity.z);
					g_notification_service->PushNotification("Flinged vehicle forward.", 1500, NotificationType::Subtitle);
				});
			}

			if (!m_InVehicle)
				ImGui::EndDisabled();


			if (ImGui::Button("Ragdoll Player")) {
				g_fiber_pool->QueueJob([](RagePlayer localPlayer, Ped localPed, Vehicle vehicle) {
					const auto player = g_player_service->GetPlayerByPeerId(g_player_service->m_selected_peer);

					if (!player)
						return;

					const auto ped = player->GetPed();

					if (!ped || !ped->m_net_object)
						return;

					g_pointers->m_request_ragdoll(ped->m_net_object->m_object_id);
					g_notification_service->PushNotification("Ragdolled player.", 1500, NotificationType::Subtitle);
				});
			}

			ImGuiWidgets::SeparatorText("Spawning");

			if (ImGui::Button("Spawn an Adder")) {
				g_fiber_pool->QueueJob([](RagePlayer player, Ped ped, Vehicle vehicle) {
					Ped targetPed;

					if (!player::GetPlayerPed(g_player_service->GetIdFromPeerId(g_player_service->m_selected_peer), &targetPed))
						return;

					constexpr auto hash = rage::joaat("adder");
					auto pos = ENTITY::GET_ENTITY_COORDS(targetPed, true);
					auto veh = vehicle::Spawn(hash, pos, 0.f, true, false);

					ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&veh);
					g_notification_service->PushNotification("Spawned vehicle.", 1500, NotificationType::Subtitle);
				});
			}

			ImGuiWidgets::SeparatorText("Vehicle");

			if (ImGui::Button("Max Upgrade Vehicle")) {
				g_fiber_pool->QueueJob([](RagePlayer player, Ped ped, Vehicle localVehicle) {
					Ped targetPed;

					if (!player::GetPlayerPed(g_player_service->GetIdFromPeerId(g_player_service->m_selected_peer), &targetPed))
						return;

					Vehicle vehicle;

					if (!vehicle::GetVehicle(targetPed, &vehicle))
						return;

					vehicle::MaxUpgradeVehicle(vehicle);

					g_notification_service->PushNotification("Max upgraded vehicle from player. ", 1500, NotificationType::Subtitle);
				});
			}

			ImGui::SameLine();

			if (ImGui::Button("Set Random Color")) {
				g_fiber_pool->QueueJob([](RagePlayer player, Ped ped, Vehicle localVehicle) {
					Ped targetPed;

					if (!player::GetPlayerPed(g_player_service->GetIdFromPeerId(g_player_service->m_selected_peer), &targetPed))
						return;

					Vehicle vehicle;

					if (!vehicle::GetVehicle(targetPed, &vehicle))
						return;

					int r = Random(0, 255);
					int g = Random(0, 255);
					int b = Random(0, 255);

					entity::TakeControlOf(vehicle, 30);
					VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(vehicle, r, g, b);

					r = Random(0, 255);
					g = Random(0, 255);
					b = Random(0, 255);

					VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(vehicle, r, g, b);

					g_notification_service->PushNotification("Max upgraded vehicle from player. ", 1500, NotificationType::Subtitle);
				});
			}
		}
		ImGui::EndChild();
	}
}