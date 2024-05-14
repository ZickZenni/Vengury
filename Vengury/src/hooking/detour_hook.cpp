#include "common.hpp"
#include "detour_hook.hpp"
#include "logger.hpp"
#include "memory/handle.hpp"
#include <..\MinHook\include\MinHook.h>

namespace Vengury
{
	DetourHook::DetourHook(std::string name, void* target, void* detour) :
		m_name(std::move(name)),
		m_target(target),
		m_detour(detour)
	{
		FixHookAddress();

		if (auto status = MH_CreateHook(m_target, m_detour, &m_original); status == MH_OK)
		{
			LOG(INFO_TO_FILE) << "Created hook '" << m_name << "'.";
		}
		else
		{
			throw std::runtime_error(fmt::format("Failed to create hook '{}' at 0x{:X} (error: {})", m_name, reinterpret_cast<std::uintptr_t>(m_target), MH_StatusToString(status)));
		}
	}

	DetourHook::~DetourHook() noexcept
	{
		if (m_target)
		{
			MH_RemoveHook(m_target);
		}

		LOG(INFO) << "Removed hook '" << m_name << "'.";
	}

	void DetourHook::Enable()
	{
		if (auto status = MH_EnableHook(m_target); status == MH_OK)
		{
			LOG(INFO_TO_FILE) << "Enabled hook '" << m_name << "'.";
		}
		else
		{
			throw std::runtime_error(fmt::format("Failed to enable hook 0x{:X} ({})", reinterpret_cast<std::uintptr_t>(m_target), MH_StatusToString(status)));
		}
	}

	void DetourHook::Disable()
	{
		if (auto status = MH_DisableHook(m_target); status == MH_OK)
		{
			LOG(INFO_TO_FILE) << "Disabled hook '" << m_name << "'.";
		}
		else
		{
			LOG(WARNING) << "Failed to disable hook '" << m_name << "'.";
		}
	}

	DWORD exp_handler(PEXCEPTION_POINTERS exp, std::string const& name)
	{
		return exp->ExceptionRecord->ExceptionCode == STATUS_ACCESS_VIOLATION
			? EXCEPTION_EXECUTE_HANDLER
			: EXCEPTION_CONTINUE_SEARCH;
	}

	void DetourHook::FixHookAddress()
	{
		__try
		{
			auto ptr = memory::Handle(m_target);
			while (ptr.as<std::uint8_t&>() == 0xE9)
			{
				ptr = ptr.add(1).rip();
			}

			m_target = ptr.as<void*>();
		}
		__except (exp_handler(GetExceptionInformation(), m_name))
		{
			[this]()
				{
					throw std::runtime_error(fmt::format("Failed to fix hook address for '{}'", m_name));
				}();
		}
	}
}
