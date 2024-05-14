#include "common.hpp"
#include "pointers.hpp"
#include "script_global.hpp"

namespace Vengury
{
	ScriptGlobal::ScriptGlobal(std::size_t index) :
		m_index(index)
	{
	}

	ScriptGlobal ScriptGlobal::At(std::ptrdiff_t index)
	{
		return ScriptGlobal(m_index + index);
	}

	ScriptGlobal ScriptGlobal::At(std::ptrdiff_t index, std::size_t size)
	{
		return ScriptGlobal(m_index + 1 + (index * size));
	}

	void *ScriptGlobal::Get()
	{
		return g_pointers->m_script_globals[m_index >> 0x12 & 0x3F] + (m_index & 0x3FFFF);
	}
}
