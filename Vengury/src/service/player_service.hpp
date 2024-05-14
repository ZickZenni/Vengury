#pragma once

#include <network/CNetGamePlayer.hpp>
#include "player.hpp"

namespace Vengury {
	using PlayerPtr = std::shared_ptr<Player>;

	class PlayerService {
	public:
		PlayerService();
		~PlayerService();

		void Tick();

		void AddPlayer(CNetGamePlayer* player);
		void RemovePlayer(CNetGamePlayer* player);

		bool IsPlaying();
		bool IsConnected();

		bool IsFriend(CNetGamePlayer* player);

		PlayerPtr GetPlayerByPeerId(uint64_t peer_id);
		PlayerPtr GetByHostToken(uint64_t token);
		CNetGamePlayer* GetNetPlayerByPeerId(uint64_t peer_id);
		uint8_t GetIdFromPeerId(uint64_t peer_id);

	public:
		uint64_t m_selected_peer = 0;

	private:
		std::map<uint64_t, PlayerPtr> m_players;
		bool m_last_connected = false;
	};

	inline PlayerService* g_player_service{};
}