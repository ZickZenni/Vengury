#pragma once

namespace Vengury
{
	class DetourHook
	{
	public:
		explicit DetourHook(std::string name, void* target, void* detour);
		~DetourHook() noexcept;

		DetourHook(DetourHook&& that) = delete;
		DetourHook& operator=(DetourHook&& that) = delete;
		DetourHook(DetourHook const&) = delete;
		DetourHook& operator=(DetourHook const&) = delete;

		void Enable();
		void Disable();

		template <typename T>
		T GetOriginal();

		void FixHookAddress();
	private:
		std::string m_name;
		void* m_target;
		void* m_detour;
		void* m_original{};
	};

	template <typename T>
	inline T DetourHook::GetOriginal()
	{
		return static_cast<T>(m_original);
	}
}
