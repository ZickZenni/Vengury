#include "common.hpp"
#include "vehicle.hpp"
#include <pointers.hpp>
#include "entity.hpp"
#include "world.hpp"
#include <gta/vehicle_values.hpp>

namespace vehicle {
	void SetMPBitset(Vehicle veh)
	{
		DECORATOR::DECOR_SET_INT(veh, "MPBitset", 0);
		auto networkId = NETWORK::VEH_TO_NET(veh);
		if (NETWORK::NETWORK_GET_ENTITY_IS_NETWORKED(veh))
			NETWORK::SET_NETWORK_ID_EXISTS_ON_ALL_MACHINES(networkId, TRUE);
		VEHICLE::SET_VEHICLE_IS_STOLEN(veh, FALSE);
	}

	void RepairVehicle(Vehicle vehicle)
	{
		entity::TakeControlOf(vehicle, 30);

		VEHICLE::SET_VEHICLE_FIXED(vehicle);
		VEHICLE::FIX_VEHICLE_WINDOW(vehicle, 0);
		VEHICLE::FIX_VEHICLE_WINDOW(vehicle, 1);
		VEHICLE::FIX_VEHICLE_WINDOW(vehicle, 2);
		VEHICLE::FIX_VEHICLE_WINDOW(vehicle, 3);
		VEHICLE::FIX_VEHICLE_WINDOW(vehicle, 4);
		VEHICLE::FIX_VEHICLE_WINDOW(vehicle, 5);
		VEHICLE::FIX_VEHICLE_WINDOW(vehicle, 6);
		VEHICLE::FIX_VEHICLE_WINDOW(vehicle, 7);
		VEHICLE::SET_VEHICLE_TYRE_FIXED(vehicle, 0);
		VEHICLE::SET_VEHICLE_TYRE_FIXED(vehicle, 1);
		VEHICLE::SET_VEHICLE_TYRE_FIXED(vehicle, 2);
		VEHICLE::SET_VEHICLE_TYRE_FIXED(vehicle, 3);
		VEHICLE::SET_VEHICLE_TYRE_FIXED(vehicle, 4);
		VEHICLE::SET_VEHICLE_TYRE_FIXED(vehicle, 5);

		ENTITY::SET_ENTITY_HEALTH(vehicle, 1000, 0, 0);
		VEHICLE::SET_VEHICLE_ENGINE_HEALTH(vehicle, 1000.f);
		VEHICLE::SET_VEHICLE_PETROL_TANK_HEALTH(vehicle, 1000.f);
		VEHICLE::SET_VEHICLE_DIRT_LEVEL(vehicle, 0.f);
	}

	void MaxUpgradeVehicle(Vehicle vehicle)
	{
		entity::TakeControlOf(vehicle, 30);
		VEHICLE::SET_VEHICLE_MOD_KIT(vehicle, 0);
		VEHICLE::TOGGLE_VEHICLE_MOD(vehicle, MOD_TURBO, TRUE);
		VEHICLE::TOGGLE_VEHICLE_MOD(vehicle, MOD_TYRE_SMOKE, TRUE);
		VEHICLE::TOGGLE_VEHICLE_MOD(vehicle, MOD_XENON_LIGHTS, TRUE);
		VEHICLE::SET_VEHICLE_WINDOW_TINT(vehicle, 1);
		VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(vehicle, false);

		for (int slot = MOD_SPOILERS; slot <= MOD_LIGHTBAR; slot++)
		{
			if (slot == MOD_LIVERY)
			{
				continue;
			}

			int count = VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, slot);
			if (count > 0)
			{
				int selected_mod = -1;

				for (int mod = count - 1; mod >= -1; mod--)
				{
					if (VEHICLE::IS_VEHICLE_MOD_GEN9_EXCLUSIVE(vehicle, slot, mod))
					{
						continue;
					}

					selected_mod = mod;
					break;
				}

				if (selected_mod != -1)
				{
					VEHICLE::SET_VEHICLE_MOD(vehicle, slot, selected_mod, true);
				}
			}
		}

		VehicleModType perfomance_mods[] = { MOD_ENGINE, MOD_BRAKES, MOD_TRANSMISSION, MOD_SUSPENSION, MOD_ARMOR, MOD_NITROUS, MOD_TURBO };

		for (auto mod_slot : perfomance_mods)
		{
			if (mod_slot != MOD_NITROUS && mod_slot != MOD_TURBO)
				VEHICLE::SET_VEHICLE_MOD(vehicle, mod_slot, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, mod_slot) - 1, true);
			else
				VEHICLE::TOGGLE_VEHICLE_MOD(vehicle, mod_slot, true);
		}

	}

	Vehicle Spawn(Hash hash, Vector3 location, float heading, bool is_networked, bool script_veh)
	{
		if (is_networked && !*Vengury::g_pointers->m_is_session_started)
			is_networked = false;

		if (entity::RequestModel(hash))
		{
			world::ApplyModelBypass();

			const auto veh = VEHICLE::CREATE_VEHICLE(hash, location.x, location.y, location.z, heading, is_networked, script_veh, false);
			
			world::RestoreModelBypass();

			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);

			if (is_networked)
			{
				SetMPBitset(veh);
			}

			return veh;
		}
		return 0;
	}

	bool GetVehicle(Ped ped, Vehicle* vehicle)
	{
		if (!PLAYER::IS_PLAYER_PLAYING(PLAYER::PLAYER_ID()))
			return false;

		if (!ENTITY::DOES_ENTITY_EXIST(ped) || !PED::IS_PED_IN_ANY_VEHICLE(ped, true))
			return false;

		const auto veh = PED::GET_VEHICLE_PED_IS_IN(ped, true);

		if (!ENTITY::DOES_ENTITY_EXIST(veh))
			return false;

		if (vehicle)
			*vehicle = veh;

		return true;
	}
}