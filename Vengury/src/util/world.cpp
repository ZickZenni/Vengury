#include "world.hpp"
#include <pointers.hpp>

namespace world {
	void ApplyModelBypass()
	{
		*(unsigned short*)Vengury::g_pointers->m_model_spawn_bypass = 0x9090;
	}

	void RestoreModelBypass()
	{
		*(unsigned short*)Vengury::g_pointers->m_model_spawn_bypass = 0x0574;
	}
}