#include "object_service.hpp"

#include "player_service.hpp"
#include <script/script.hpp>
#include <fiber_pool.hpp>
#include <util/entity.hpp>

namespace Vengury {
	ObjectService::ObjectService()
	{
		g_object_service = this;
	}

	ObjectService::~ObjectService()
	{
		Cleanup();
		g_object_service = nullptr;
	}

	Object ObjectService::Create(Hash hash, float x, float y, float z, bool dynamic)
	{
		if (!g_player_service->IsPlaying())
			return 0;

		bool isNetworked = g_player_service->IsConnected();

		if (!STREAMING::HAS_MODEL_LOADED(hash)) {
			STREAMING::REQUEST_MODEL(hash);
			while (!STREAMING::HAS_MODEL_LOADED(hash)) {
				Script::GetCurrent()->YieldScript();
			}
		}

		auto object = OBJECT::CREATE_OBJECT(hash, x, y, z, isNetworked, false, dynamic);
		STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);

		if (isNetworked) {
			auto networkId = NETWORK::OBJ_TO_NET(object);
			if (NETWORK::NETWORK_GET_ENTITY_IS_NETWORKED(object))
				NETWORK::SET_NETWORK_ID_EXISTS_ON_ALL_MACHINES(networkId, TRUE);
		}

		m_objects.push_back(object);
		return object;
	}

	void ObjectService::Delete(Object object)
	{
		if (g_player_service->IsPlaying() && ENTITY::DOES_ENTITY_EXIST(object)) {
			entity::TakeControlOf(object, 30);
			ENTITY::SET_ENTITY_AS_MISSION_ENTITY(object, true, true);
			ENTITY::DELETE_ENTITY(&object);
			ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&object);
		}
		m_objects.erase(std::remove(m_objects.begin(), m_objects.end(), object), m_objects.end());
	}

	void ObjectService::Cleanup()
	{
		for (const auto& object : m_objects) {
			Delete(object);
		}
		m_objects.clear();
	}
}
