#pragma once

#include "gta/net_game_event.hpp"
#include "network/CNetGamePlayer.hpp"
#include "service/player_service.hpp"

namespace Vengury
{
	class Packet
	{
	public:
		char m_data[0x400]{};
		rage::datBitBuffer m_buffer;

		Packet();
		void Send(uint32_t msg_id);
		void Send(PlayerPtr player, int connection_id);
		void Send(int peer_id, int connection_id);

		inline operator rage::datBitBuffer& ()
		{
			return m_buffer;
		}

		template<typename T>
		inline void write(T data, int length)
		{
			m_buffer.Write<T>(data, length);
		}

		inline void write_message(rage::eNetMessage message)
		{
			write<int>(0x3246, 14);
			if ((int)message > 0xFF)
			{
				write<bool>(true, 1);
				write<rage::eNetMessage>(message, 16);
			}
			else
			{
				write<bool>(false, 1);
				write<rage::eNetMessage>(message, 8);
			}
		}

		inline void write_peer_id(uint64_t peer_id)
		{
			char b[8];
			rage::datBitBuffer buf(b, 8);
			buf.WriteQWord(peer_id, 64);
			m_buffer.WriteArray(b, 8 * buf.GetDataLength());
		}
	};
}