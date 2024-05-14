#include "../common.hpp"
#include "module.hpp"

namespace memory
{
	Module::Module(HMODULE mod) :
		Range(mod, 0)
	{
		auto dosHeader = m_base.as<IMAGE_DOS_HEADER*>();
		auto ntHeader = m_base.add(dosHeader->e_lfanew).as<IMAGE_NT_HEADERS*>();
		m_size = ntHeader->OptionalHeader.SizeOfImage;
	}

	Module::Module(std::nullptr_t) :
		Module(GetModuleHandle(nullptr))
	{
	}

	Module::Module(std::string_view name) :
		Module(GetModuleHandleA(name.data()))
	{
	}

	Module::Module(std::wstring_view name) :
		Module(GetModuleHandleW(name.data()))
	{
	}

	Handle Module::GetExport(std::string_view symbol_name)
	{
		return GetProcAddress(m_base.as<HMODULE>(), symbol_name.data());
	}
}
