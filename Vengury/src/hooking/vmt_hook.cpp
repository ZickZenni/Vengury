#include "vmt_hook.hpp"

#include "common.hpp"

namespace Vengury
{
	VmtHook::VmtHook(void* obj, std::size_t num_funcs) :
		m_object(static_cast<void***>(obj)),
		m_num_funcs(num_funcs),
		m_original_table(*m_object),
		m_new_table(std::make_unique<void* []>(m_num_funcs))
	{
		std::copy_n(m_original_table, m_num_funcs, m_new_table.get());
	}

	VmtHook::~VmtHook()
	{
		Disable();
	}

	void VmtHook::Hook(std::size_t index, void* func)
	{
		m_new_table[index] = func;
	}

	void VmtHook::Unhook(std::size_t index)
	{
		m_new_table[index] = m_original_table[index];
	}

	void VmtHook::Enable()
	{
		*m_object = m_new_table.get();
	}

	void VmtHook::Disable()
	{
		*m_object = m_original_table;
	}
}
