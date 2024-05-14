#include "common.hpp"
#include "entity.hpp"
#include "script/script.hpp"
#include <fiber_pool.hpp>
#include <pointers.hpp>
#include "vehicle.hpp"
#include "service/player_service.hpp"

namespace entity {
	bool RequestModel(rage::joaat_t hash)
	{
		if (STREAMING::HAS_MODEL_LOADED(hash))
		{
			return true;
		}

		bool has_loaded;

		if (STREAMING::IS_MODEL_VALID(hash) && STREAMING::IS_MODEL_IN_CDIMAGE(hash))
		{
			do
			{
				has_loaded = STREAMING::HAS_MODEL_LOADED(hash);
				if (has_loaded)
					break;

				STREAMING::REQUEST_MODEL(hash);

				Vengury::Script::GetCurrent()->YieldScript();
			} while (!has_loaded);

			return true;
		}

		return false;
	}

	bool LoadGroundAt3DCoords(Vector3 location, float* endHeight)
	{
		constexpr float maxGroundCheck = 1000.f;
		constexpr int maxAttempts = 300;
		float groundZ = location.z;
		int attempts = 0;
		bool groundFound;
		float height;

		do {
			groundFound = MISC::GET_GROUND_Z_FOR_3D_COORD(location.x, location.y, maxGroundCheck, &groundZ, FALSE, FALSE);
			STREAMING::REQUEST_COLLISION_AT_COORD(location.x, location.y, location.z);

			if (attempts % 10 == 0)
			{
				location.z += 25.f;
			}

			++attempts;

			Vengury::Script::GetCurrent()->YieldScript();
		} while (!groundFound && attempts < maxAttempts);

		if (!groundFound)
		{
			return false;
		}

		if (WATER::GET_WATER_HEIGHT(location.x, location.y, location.z, &height))
		{
			location.z = height;
		}
		else
		{
			location.z = groundZ + 1.f;
		}

		if (endHeight)
			*endHeight = location.z;

		return true;
	}

	bool DeleteEntity(Entity entity)
	{
		if (!ENTITY::DOES_ENTITY_EXIST(entity))
			return false;

		if (!TakeControlOf(entity, 30)) {
			LOG(WARNING) << "Failed to take control of entity before deleting";
			return false;
		}

		ENTITY::DETACH_ENTITY(entity, 1, 1);
		ENTITY::SET_ENTITY_COORDS_NO_OFFSET(entity, 7000.f, 7000.f, 15.f, 0, 0, 0);
		if (!ENTITY::IS_ENTITY_A_MISSION_ENTITY(entity))
		{
			ENTITY::SET_ENTITY_AS_MISSION_ENTITY(entity, true, true);
		}
		ENTITY::DELETE_ENTITY(&entity);
		return true;
	}

	void AttachEntityToEntity(Entity entity, Entity target, int boneIndex, float xPos, float yPos, float zPos, float xRot, float yRot, float zRot, bool useSoftPinning, bool collision, bool isPed, int vertexIndex, bool fixedRot)
	{
		if (!ENTITY::DOES_ENTITY_EXIST(entity) || !ENTITY::DOES_ENTITY_EXIST(target))
			return;

		TakeControlOf(entity, 30);
		TakeControlOf(target, 30);
		ENTITY::ATTACH_ENTITY_TO_ENTITY(entity, target, boneIndex, xPos, yPos, zPos, xRot, yRot, zRot, false, useSoftPinning, collision, isPed, vertexIndex, fixedRot, false);
	}

	bool TeleportEntityToCoords(Entity entity, Vector3 location, bool checkGround, bool includeVehicle)
	{
		if (!ENTITY::DOES_ENTITY_EXIST(entity))
			return false;

		if (ENTITY::IS_ENTITY_A_PED(entity) && includeVehicle) {
			Vehicle vehicle;

			if (vehicle::GetVehicle(entity, &vehicle)) {
				return TeleportEntityToCoords(vehicle, location, checkGround, false);
			}
		}

		if (!entity::LoadGroundAt3DCoords(location, checkGround ? &location.z : nullptr))
			return false;

		if (ENTITY::IS_ENTITY_A_VEHICLE(entity) && *Vengury::g_pointers->m_is_session_started)
			if (!entity::TakeControlOf(entity, 30))
				return false;

		ENTITY::SET_ENTITY_COORDS(entity, location.x, location.y, location.z, 1, 0, 0, 0);
		return true;
	}

	bool NetworkHasControlOfEntity(rage::netObject* object)
	{
		return !object || !object->m_next_owner_id && (object->m_control_id == -1);
	}

	bool TakeControlOf(Entity entity, int timeout)
	{
		using namespace Vengury;

		if (!g_player_service->IsConnected())
			return true;

		auto hnd = g_pointers->m_handle_to_ptr(entity);

		if (!hnd || !hnd->m_net_object || !g_player_service->IsConnected())
			return false;

		if (NetworkHasControlOfEntity(hnd->m_net_object))
			return true;

		for (int i = 0; i < timeout; i++)
		{
			g_pointers->m_request_control(hnd->m_net_object);

			if (NetworkHasControlOfEntity(hnd->m_net_object))
				return true;

			if (timeout != 0)
				Script::GetCurrent()->YieldScript();
		}

		return false;
	}

}