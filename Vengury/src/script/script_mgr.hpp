#pragma once
#include "common.hpp"
#include "script.hpp"

namespace Vengury
{
	class ScriptManager
	{
	public:
		explicit ScriptManager() = default;
		~ScriptManager() = default;

		void AddScript(std::unique_ptr<Script> script);
		void RemoveAllScripts();

		void Tick();
	private:
		void Tick_internal();
	private:
		std::recursive_mutex m_mutex;
		std::vector<std::unique_ptr<Script>> m_scripts;
	};

	inline ScriptManager g_script_mgr;
}
