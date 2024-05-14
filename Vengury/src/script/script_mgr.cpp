#include "common.hpp"
#include <rage/atArray.hpp>
#include <script/scrThread.hpp>
#include <script/tlsContext.hpp>
#include "util/gta_util.hpp"
#include "invoker.hpp"
#include "pointers.hpp"
#include "script_mgr.hpp"

namespace Vengury
{
	void ScriptManager::AddScript(std::unique_ptr<Script> script)
	{
		std::lock_guard lock(m_mutex);
		m_scripts.push_back(std::move(script));
	}

	void ScriptManager::RemoveAllScripts()
	{
		std::lock_guard lock(m_mutex);
		m_scripts.clear();
	}

	void ScriptManager::Tick()
	{
		gta_util::ExecuteAsScript(rage::joaat("main_persistent"), std::mem_fn(&ScriptManager::Tick_internal), this);
	}

	void ScriptManager::Tick_internal()
	{
		static bool ensure_main_fiber = (ConvertThreadToFiber(nullptr), true);
		static bool ensure_native_handlers = (g_native_invoker.CacheHandlers(), true);

		std::lock_guard lock(m_mutex);
		for (auto const &script : m_scripts)
		{
			script->Tick();
		}
	}
}
