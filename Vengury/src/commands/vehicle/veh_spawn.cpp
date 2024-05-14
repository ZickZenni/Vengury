#include "veh_spawn.hpp"
#include <gta/natives.hpp>
#include <util/vehicle.hpp>
#include <util/player_util.hpp>

namespace Vengury {
	VehSpawnCommand::VehSpawnCommand() : Command("vehspawn")
	{
	}

	bool VehSpawnCommand::Execute(PlayerPtr sender, std::vector<std::string> args)
	{
		if (args.size() < 1)
			return false;

		const auto& input = args.at(0);
		const auto hash = rage::joaat(input);

		if (!STREAMING::IS_MODEL_VALID(hash)) {
			return false;
		}

		Ped ped;

		if (!player::GetPlayerPed(sender->GetId(), &ped)) {
			LOG(WARNING) << "Failed to get player ped while executing command.";
			return false;
		}

		auto pos = ENTITY::GET_ENTITY_COORDS(ped, true);
		auto veh = vehicle::Spawn(hash, pos, 0.f, true, false);

		ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&veh);
		return true;
	}
}
