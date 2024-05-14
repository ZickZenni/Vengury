#pragma once
#include "common.hpp"

namespace Vengury
{
	class ScriptLocal
	{
	public:
		explicit ScriptLocal(rage::scrThread* thread, std::size_t index);
		explicit ScriptLocal(PVOID stack, std::size_t index);

		ScriptLocal At(std::ptrdiff_t index);
		ScriptLocal At(std::ptrdiff_t index, std::size_t size);

		template <typename T>
		std::enable_if_t<std::is_pointer_v<T>, T> As()
		{
			return static_cast<T>(Get());
		}

		template <typename T>
		std::enable_if_t<std::is_lvalue_reference_v<T>, T> As()
		{
			return *static_cast<std::add_pointer_t<std::remove_reference_t<T>>>(Get());
		}
	private:
		void *Get();
		std::size_t m_index;
		PVOID m_stack;
	};
}
