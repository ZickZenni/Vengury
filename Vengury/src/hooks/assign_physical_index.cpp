#include <hooking/hooking.hpp>

#include <network/CNetGamePlayer.hpp>
#include <network/CNetworkPlayerMgr.hpp>
#include <service/player_service.hpp>
#include <service/notification_service.hpp>
#include <pointers.hpp>

namespace Vengury {
	void* Hooks::assign_physical_index(CNetworkPlayerMgr* netPlayerMgr, CNetGamePlayer* player, uint8_t new_index)
	{
		auto* netData = player->get_net_data();

		if (!netData) {
			return g_hooking->m_assign_physical_index.GetOriginal<decltype(&assign_physical_index)>()(netPlayerMgr, player, new_index);
		}

		// Leaving
		if (new_index == 255) {
			if (g_player_service->GetPlayerByPeerId(netData->m_peer_id)) {
				g_player_service->RemovePlayer(player);

				LOG(INFO) << "Player left '" << netData->m_name << "' freeing slot #" << (int)player->m_player_id
					<< " with Rockstar ID: " << netData->m_gamer_handle.m_rockstar_id;
			}
			return g_hooking->m_assign_physical_index.GetOriginal<decltype(&assign_physical_index)>()(netPlayerMgr, player, new_index);
		}

		const auto result = g_hooking->m_assign_physical_index.GetOriginal<decltype(&assign_physical_index)>()(netPlayerMgr, player, new_index);

		if (g_player_service->GetPlayerByPeerId(netData->m_peer_id))
			return result;

		g_player_service->AddPlayer(player);

		if (g_player_service->IsFriend(player))
			g_notification_service->Push("Friends", std::format("A friend '{}' has joined your session.", netData->m_name), 3500);

		LOG(INFO) << "Player joined '" << netData->m_name << "' allocating slot #" << (int)player->m_player_id
			<< " with Rockstar ID: " << netData->m_gamer_handle.m_rockstar_id;

		g_notification_service->PushNotification(std::format("<C>{}</C> joined the game.", netData->m_name).c_str(), 0, NotificationType::Minimap);
		return result;
	}
}