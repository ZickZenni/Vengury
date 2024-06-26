#include "packet.hpp"

#include "common.hpp"
#include "util/gta_util.hpp"

#include <network/Network.hpp>
#include <network/netConnection.hpp>

namespace Vengury
{
	Packet::Packet() :
		m_buffer(m_data, sizeof(m_data))
	{
	}

	void Packet::Send(uint32_t msg_id)
	{
		g_pointers->m_queue_packet(gta_util::GetNetwork()->m_game_session_ptr->m_net_connection_mgr, msg_id, m_data, (m_buffer.m_curBit + 7) >> 3, 1, nullptr);
	}

	void Packet::Send(PlayerPtr player, int connection_id)
	{
		Send(player->GetSessionPlayer()->m_player_data.m_peer_id_2, connection_id);
	}

	void Packet::Send(int peer_id, int connection_id)
	{
		auto mgr = gta_util::GetNetwork()->m_game_session_ptr->m_net_connection_mgr;
		auto peer = g_pointers->m_get_connection_peer(mgr, peer_id);
		g_pointers->m_send_packet(mgr, &peer->m_peer_address, connection_id, m_data, (m_buffer.m_curBit + 7) >> 3, 0x1000000);
	}
}