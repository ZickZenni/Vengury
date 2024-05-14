#pragma once

#include "gta/natives.hpp"

namespace player {
	bool GetPlayerPed(Player player, Ped* player_ped);

	void SendSubtitle(const char* text, int duration);
}