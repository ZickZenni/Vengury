#pragma once

#include "gta/natives.hpp"

namespace vehicle {
	void SetMPBitset(Vehicle vehicle);

	void RepairVehicle(Vehicle vehicle);
	void MaxUpgradeVehicle(Vehicle vehicle);

	Vehicle Spawn(Hash hash, Vector3 location, float heading, bool is_networked, bool script_veh);

	bool GetVehicle(Ped ped, Vehicle* vehicle);
}