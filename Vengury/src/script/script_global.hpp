#pragma once
#include "common.hpp"

namespace Vengury
{
	class ScriptGlobal
	{
	public:
		explicit ScriptGlobal(std::size_t index);

		ScriptGlobal At(std::ptrdiff_t index);
		ScriptGlobal At(std::ptrdiff_t index, std::size_t size);

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
		void* Get();
		std::size_t m_index;
	};
}
