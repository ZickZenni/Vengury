#pragma once
#include "common.hpp"

namespace Vengury
{
	class Script
	{
	public:
		using func_t = void(*)();
	public:
		explicit Script(func_t func, std::optional<std::size_t> stack_size = std::nullopt);
		~Script();

		void Tick();
		void YieldScript(std::optional<std::chrono::high_resolution_clock::duration> time = std::nullopt);
		static Script* GetCurrent();
		static void ScriptExceptionHandler(PEXCEPTION_POINTERS exp);
	private:
		void FiberFunc();
	private:
		void* m_script_fiber;
		void* m_main_fiber;
		func_t m_func;
		std::optional<std::chrono::high_resolution_clock::time_point> m_wake_time;
	};


#define TRY_CLAUSE  __try
#define EXCEPT_CLAUSE  __except (Script::ScriptExceptionHandler(GetExceptionInformation()), EXCEPTION_EXECUTE_HANDLER) { }
}
