#pragma once

#include <network/CNetGamePlayer.hpp>
#include <network/snSession.hpp>

namespace Vengury {
	class Player {
	public:
		explicit Player(CNetGamePlayer* net_player);
		~Player();

		CNetGamePlayer* GetNetPlayer() const;
		rage::rlGamerInfo* GetNetData() const;
		rage::snPlayer* GetSessionPlayer();

		CPed* GetPed() const;
		CPlayerInfo* GetPlayerInfo() const;

		uint8_t GetId() const;

		bool IsFriend();

	private:
		CNetGamePlayer* m_net_player;
	};
}