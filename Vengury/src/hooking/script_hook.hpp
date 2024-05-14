#pragma once
#include "common.hpp"
#include <script/scrNativeHandler.hpp>
#include <script/scrProgram.hpp>
#include <rage/joaat.hpp>
#include "vmt_hook.hpp"

namespace Vengury
{
	class ScriptHook
	{
	public:
		explicit ScriptHook(rage::joaat_t script_hash, std::unordered_map<rage::scrNativeHash, rage::scrNativeHandler> native_replacements);
		~ScriptHook();

		void Ensure();
	private:
		void HookInstance(rage::scrProgram *program);

		static void scrprogram_dtor(rage::scrProgram *this_, bool free_memory);
		static std::unordered_map<rage::scrProgram*, ScriptHook*> s_map;

		rage::joaat_t m_script_hash;
		rage::scrProgram* m_program;
		std::unique_ptr<VmtHook> m_vmt_hook;
		std::unordered_map<rage::scrNativeHash, rage::scrNativeHandler> m_native_replacements;
		std::unordered_map<rage::scrNativeHash, rage::scrNativeHandler*> m_native_handler_ptrs;
	};
}
