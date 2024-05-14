#include "features.hpp"

#include "common.hpp"

#include "logger.hpp"
#include "script/script.hpp"
#include "service/player_service.hpp"
#include "util/gta_util.hpp"
#include "gta/natives.hpp"
#include "gta/joaat.hpp"

namespace Vengury
{
	void features::RunTick()
	{
		if (!*g_pointers->m_network_player_mgr || !(*g_pointers->m_network_player_mgr)->m_local_net_player
			|| (*g_pointers->m_network_player_mgr)->m_local_net_player->m_player_id == -1) [[unlikely]]
			self::id = 0;
		else [[likely]]
			self::id = (*g_pointers->m_network_player_mgr)->m_local_net_player->m_player_id;

			self::ped = PLAYER::PLAYER_PED_ID();

			STATS::STAT_GET_INT("MPPLY_LAST_MP_CHAR"_J, &self::char_index, true);

			self::pos = ENTITY::GET_ENTITY_COORDS(self::ped, false /*Unused*/);

			self::rot = ENTITY::GET_ENTITY_ROTATION(self::ped, 2);

			if (PED::IS_PED_IN_ANY_VEHICLE(self::ped, 0))
			{
				self::veh = PED::GET_VEHICLE_PED_IS_IN(self::ped, false);
			}
			else
			{
				self::veh = 0;
			}

		g_local_player = gta_util::GetLocalPed();
		g_player_service->Tick();
	}

	void features::ScriptFunc()
	{
		while (true)
		{
			g_player_service->Tick();
			Script::GetCurrent()->YieldScript();
		}
	}
}
