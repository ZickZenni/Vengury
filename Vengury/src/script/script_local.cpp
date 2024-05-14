#include "common.hpp"
#include "pointers.hpp"
#include "script_local.hpp"
#include <script/scrThread.hpp>

namespace Vengury
{
	ScriptLocal::ScriptLocal(rage::scrThread* thread, std::size_t index) :
		m_index(index), m_stack(thread->m_stack)
	{
	}
	ScriptLocal::ScriptLocal(PVOID stack, std::size_t index) :
		m_index(index), m_stack(stack)
	{
	}

	ScriptLocal ScriptLocal::At(std::ptrdiff_t index)
	{
		return ScriptLocal(m_stack, m_index + index);
	}

	ScriptLocal ScriptLocal::At(std::ptrdiff_t index, std::size_t size)
	{
		return ScriptLocal(m_stack, m_index + 1 + (index * size));
	}

	void *ScriptLocal::Get()
	{
		return reinterpret_cast<uintptr_t*>((uintptr_t)m_stack + (m_index * sizeof(uintptr_t)));
	}
}
