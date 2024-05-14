#pragma once

#include <rage/joaat.hpp>
#include "gta/natives.hpp"

namespace entity {
	bool RequestModel(rage::joaat_t hash);

	bool LoadGroundAt3DCoords(Vector3 location, float* endHeight);

	bool DeleteEntity(Entity entity);

	void AttachEntityToEntity(Entity entity, Entity target, int boneIndex,
		float xPos,
		float yPos,
		float zPos,
		float xRot,
		float yRot,
		float zRot,
		bool useSoftPinning,
		bool collision,
		bool isPed,
		int vertexIndex,
		bool fixedRot);

	bool TeleportEntityToCoords(Entity entity, Vector3 location, bool checkGround, bool includeVehicle);

	bool NetworkHasControlOfEntity(rage::netObject* object);
	bool TakeControlOf(Entity entity, int timeout);
}