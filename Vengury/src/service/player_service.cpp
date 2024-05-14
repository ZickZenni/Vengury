#include "player_service.hpp"

#include "util/gta_util.hpp"
#include "hooking/hooking.hpp"
#include "notification_service.hpp"

#include <format>

namespace Vengury {
	PlayerService::PlayerService()
	{
		g_player_service = this;

		if (!IsConnected())
			return;

		const auto mgr = gta_util::GetNetworkPlayerManager();
		if (!mgr)
			return;

		const auto localPlayer = reinterpret_cast<CNetGamePlayer*>(mgr->m_local_net_player);

		if (!localPlayer)
			return;

		if (!localPlayer->get_net_data())
			return;

		for (uint16_t i = 0; i < mgr->m_player_limit; ++i) {
			const auto player = reinterpret_cast<CNetGamePlayer*>(mgr->m_player_list[i]);

			if (!player)
				continue;

			const auto netData = player->get_net_data();

			if (!netData)
				continue;

			// Ignore local player
			if (netData->m_peer_id == localPlayer->get_net_data()->m_peer_id)
				continue;

			AddPlayer(player);
		}
	}

	PlayerService::~PlayerService()
	{
		g_player_service = nullptr;
	}

	void PlayerService::Tick()
	{
		if (!IsConnected()) {
			if (!m_last_connected)
				return;

			LOG(INFO) << "Disconnected from session.";

			m_last_connected = false;
			return;
		}

		const auto mgr = gta_util::GetNetworkPlayerManager();
		if (!mgr)
			return;

		const auto localPlayer = reinterpret_cast<CNetGamePlayer*>(mgr->m_local_net_player);

		if (!localPlayer)
			return;

		if (!localPlayer->get_net_data())
			return;

		if (!m_last_connected) {
			LOG(INFO) << "Connected to a new session.";

			g_notification_service->Push("Session", std::format("Joined session with players {} / {}.", mgr->m_player_count, mgr->m_player_limit), 3000);

			m_last_connected = true;
		}

		/*if (!IsConnected()) {
			if (!m_LastConnected)
				return;

			LOG(INFO) << "Disconnected from session.";

			m_LastConnected = false;
			return;
		}

		const auto mgr = gta_util::GetNetworkPlayerManager();
		if (!mgr)
			return;

		const auto localPlayer = reinterpret_cast<CNetGamePlayer*>(mgr->m_local_net_player);

		if (!localPlayer)
			return;

		if (!localPlayer->get_net_data())
			return;

		if (!m_LastConnected) {
			LOG(INFO) << "Connected to a new session.";

			m_LastConnected = true;
		}

		// Add new players
		for (uint16_t i = 0; i < mgr->m_player_limit; ++i) {
			const auto player = reinterpret_cast<CNetGamePlayer*>(mgr->m_player_list[i]);

			if (!player)
				continue;

			const auto netData = player->get_net_data();

			if (!netData)
				continue;

			// Ignore local player
			if (netData->m_peer_id == localPlayer->get_net_data()->m_peer_id)
				continue;

			// Player is already in list
			if (m_Players.find(netData->m_peer_id) != m_Players.end())
				continue;

			// Add player to list
			auto ply = std::make_shared<Player>(player);
			m_Players.insert({ netData->m_peer_id, std::move(ply) });

			//LOG(INFO) << "Player joined '" << netData->m_name << "' allocating slot #" << (int)player->m_player_id
			//	<< " with Rockstar ID: " << netData->m_gamer_handle.m_rockstar_id;
		}

		// Remove disconnected players
		for (const auto& [peer_id, player] : m_Players) {
			bool connected = false;

			for (uint16_t i = 0; i < mgr->m_player_limit; ++i) {
				const auto netPlayer = reinterpret_cast<CNetGamePlayer*>(mgr->m_player_list[i]);

				if (!netPlayer)
					continue;

				const auto netData = netPlayer->get_net_data();

				if (!netData)
					continue;

				if (peer_id == netData->m_peer_id) {
					connected = true;
					break;
				}
			}

			if (connected)
				continue;

			//LOG(INFO) << "Player left, freeing slot #" << (int)player->GetId()
			//	<< " with Rockstar ID: " << player->GetRockstarId();

			m_Players.erase(peer_id);
		}*/
	}

	void PlayerService::AddPlayer(CNetGamePlayer* player)
	{
		if (!player)
			return;

		const auto netData = player->get_net_data();

		if (!netData)
			return;

		if (GetPlayerByPeerId(netData->m_peer_id))
			return;

		// Add player to list
		auto ply = std::make_shared<Player>(player);
		m_players.insert({ netData->m_peer_id, std::move(ply) });
	}

	void PlayerService::RemovePlayer(CNetGamePlayer* player)
	{
		if (!player)
			return;

		const auto netData = player->get_net_data();

		if (!netData)
			return;

		if (auto it = std::find_if(m_players.begin(),
			m_players.end(),
			[player](const auto& p) {
				return p.second->GetId() == player->m_player_id;
			});
			it != m_players.end())
		{
			m_players.erase(it);
		}
	}

	bool PlayerService::IsPlaying()
	{
		return ENTITY::DOES_ENTITY_EXIST(PLAYER::PLAYER_PED_ID());
	}

	bool PlayerService::IsConnected()
	{
		return *g_pointers->m_is_session_started;
	}

	bool PlayerService::IsFriend(CNetGamePlayer* player)
	{
		if (!player)
			return false;

		if (const auto netData = player->get_net_data()) {
			const auto rockstar_id = netData->m_gamer_handle.m_rockstar_id;
			for (uint32_t i = 0; i < g_pointers->m_friend_registry->m_friend_count; i++)
				if (rockstar_id == g_pointers->m_friend_registry->get(i)->m_rockstar_id)
					return true;
		}
		return false;
	}

	PlayerPtr PlayerService::GetPlayerByPeerId(uint64_t peer_id)
	{
		if (m_players.find(peer_id) == m_players.end())
			return nullptr;
		return m_players[peer_id];
	}

	PlayerPtr PlayerService::GetByHostToken(uint64_t token)
	{
		for (const auto& [_, player] : m_players)
		{
			if (auto net_data = player->GetNetData())
			{
				if (!net_data)
					continue;

				if (net_data && net_data->m_host_token == token)
				{
					return player;
				}
			}
		}
		return nullptr;
	}

	CNetGamePlayer* PlayerService::GetNetPlayerByPeerId(uint64_t peer_id)
	{
		if (!IsConnected())
			return nullptr;

		const auto mgr = gta_util::GetNetworkPlayerManager();
		if (!mgr)
			return nullptr;

		for (uint16_t i = 0; i < mgr->m_player_limit; ++i) {
			const auto player = reinterpret_cast<CNetGamePlayer*>(mgr->m_player_list[i]);

			if (!player)
				continue;

			const auto netData = player->get_net_data();

			if (!netData)
				continue;

			// Ignore local player
			if (netData->m_peer_id == peer_id)
				return player;
		}

		return nullptr;
	}

	uint8_t PlayerService::GetIdFromPeerId(uint64_t peer_id)
	{
		const auto player = GetNetPlayerByPeerId(peer_id);
		
		if (!player)
			return 0;

		return player->m_player_id;
	}
}