#include "vtable_hook.hpp"

namespace Vengury
{
	VTableHook::VTableHook(void** vft, std::size_t num_funcs) :
		m_num_funcs(num_funcs),
		m_table(vft),
		m_backup_table(std::make_unique<void* []>(m_num_funcs)),
		m_hook_table(std::make_unique<void* []>(m_num_funcs))
	{
		std::memcpy(m_backup_table.get(), m_table, m_num_funcs * sizeof(void*));
		std::memcpy(m_hook_table.get(), m_table, m_num_funcs * sizeof(void*));
	}

	VTableHook::~VTableHook()
	{
		Disable();
	}

	void VTableHook::Hook(std::size_t index, void* func)
	{
		m_hook_table[index] = func;
	}

	void VTableHook::Unhook(std::size_t index)
	{
		m_hook_table[index] = m_backup_table[index];
	}

	void VTableHook::Enable()
	{
		std::memcpy(m_table, m_hook_table.get(), m_num_funcs * sizeof(void*));
	}

	void VTableHook::Disable()
	{
		std::memcpy(m_table, m_backup_table.get(), m_num_funcs * sizeof(void*));
	}
}
