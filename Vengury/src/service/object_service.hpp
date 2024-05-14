#pragma once

#include <gta/natives.hpp>

namespace Vengury {
	class ObjectService {
	public:
		ObjectService();
		~ObjectService();

		Object Create(Hash hash, float x, float y, float z, bool dynamic);
		void Delete(Object object);

		void Cleanup();

	private:
		std::vector<Object> m_objects{};
	};

	inline ObjectService* g_object_service{};
}