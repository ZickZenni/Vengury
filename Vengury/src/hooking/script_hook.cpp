#include "common.hpp"
#include "gta/crossmap.hpp"
#include <script/scrProgram.hpp>
#include <script/scrProgramTable.hpp>
#include "logger.hpp"
#include "pointers.hpp"
#include "script_hook.hpp"

namespace Vengury
{
	inline std::unordered_map<rage::scrProgram*, ScriptHook*> ScriptHook::s_map;

	static bool map_native(rage::scrNativeHash* hash)
	{
		for (auto const& mapping : g_crossmap)
		{
			if (mapping.first == *hash)
			{
				*hash = mapping.second;
				return true;
			}
		}

		return false;
	}

	ScriptHook::ScriptHook(rage::joaat_t script_hash, std::unordered_map<rage::scrNativeHash, rage::scrNativeHandler> native_replacements) :
		m_script_hash(script_hash),
		m_native_replacements(std::move(native_replacements))
	{
		Ensure();
	}

	ScriptHook::~ScriptHook()
	{
		if (m_program)
		{
			for (auto [hash, handler_ptr] : m_native_handler_ptrs)
			{
				auto og_handler = g_pointers->m_get_native_handler(g_pointers->m_native_registration_table, hash);
				*handler_ptr = og_handler;
			}
		}

		if (m_vmt_hook)
		{
			m_vmt_hook->Disable();
			s_map.erase(m_program);
		}
	}

	void ScriptHook::Ensure()
	{
		if (m_vmt_hook)
			return;

		if (auto program = g_pointers->m_script_program_table->find_script(m_script_hash))
		{
			if (program->is_valid())
			{
				HookInstance(program);
				LOG(INFO) << "Hooked " << program->m_name << " script (" << HEX_TO_UPPER(static_cast<void*>(program)) << ")";
			}
		}
	}

	void ScriptHook::HookInstance(rage::scrProgram* program)
	{
		m_program = program;
		s_map.emplace(m_program, this);
		m_vmt_hook = std::make_unique<VmtHook>(m_program, 3);
		m_vmt_hook->Hook(0, &scrprogram_dtor);

		for (auto [replacement_hash, replacement_handler] : m_native_replacements)
		{
			auto hash = replacement_hash;
			map_native(&hash);

			auto og_handler = g_pointers->m_get_native_handler(g_pointers->m_native_registration_table, hash);
			if (!og_handler)
				continue;

			auto handler_ptr = m_program->get_address_of_native_entrypoint(og_handler);
			if (!handler_ptr)
				continue;

			m_native_handler_ptrs.emplace(hash, reinterpret_cast<rage::scrNativeHandler*>(handler_ptr));
			*handler_ptr = replacement_handler;
		}
	}

	void ScriptHook::scrprogram_dtor(rage::scrProgram* this_, bool free_memory)
	{
		if (auto it = s_map.find(this_); it != s_map.end())
		{
			auto hook = it->second;

			hook->m_program = nullptr;
			s_map.erase(it);

			auto og_func = hook->m_vmt_hook->GetOriginal<decltype(&scrprogram_dtor)>(0);
			hook->m_vmt_hook->Disable();
			hook->m_vmt_hook.reset();

			og_func(this_, free_memory);
		}
	}
}
