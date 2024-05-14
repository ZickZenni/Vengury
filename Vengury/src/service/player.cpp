#include "player.hpp"
#include <util/gta_util.hpp>
#include <pointers.hpp>

namespace Vengury {
	Player::Player(CNetGamePlayer* net_player)
	{
		m_net_player = net_player;
	}

	Player::~Player()
	{

	}

	CNetGamePlayer* Player::GetNetPlayer() const
	{
		return m_net_player;
	}

	rage::rlGamerInfo* Player::GetNetData() const
	{
		if (!m_net_player)
			return nullptr;
		return m_net_player->get_net_data();
	}

	rage::snPlayer* Player::GetSessionPlayer()
	{
		const auto network = gta_util::GetNetwork();

		for (uint32_t i = 0; i < network->m_game_session_ptr->m_player_count; i++)
		{
			if (network->m_game_session_ptr->m_players[i]->m_player_data.m_host_token == m_net_player->get_net_data()->m_host_token)
			{
				return network->m_game_session_ptr->m_players[i];
			}
		}

		return nullptr;
	}

	CPed* Player::GetPed() const
	{
		if (const auto player_info = this->GetPlayerInfo(); player_info != nullptr)
			if (const auto ped = player_info->m_ped; ped != nullptr)
				return ped;
		return nullptr;
	}

	CPlayerInfo* Player::GetPlayerInfo() const
	{
		if (auto net_player = m_net_player)
			return net_player->m_player_info;
		return nullptr;
	}

	uint8_t Player::GetId() const
	{
		if (m_net_player)
			return m_net_player->m_player_id;
		return 0;
	}

	bool Player::IsFriend()
	{
		if (!m_net_player)
			return false;

		if (const auto netData = m_net_player->get_net_data()) {
			const auto rockstar_id = netData->m_gamer_handle.m_rockstar_id;
			for (uint32_t i = 0; i < g_pointers->m_friend_registry->m_friend_count; i++)
				if (rockstar_id == g_pointers->m_friend_registry->get(i)->m_rockstar_id)
					return true;
		}
		return false;
	}
}