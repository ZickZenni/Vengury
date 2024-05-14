#pragma once

namespace Vengury
{
	class VmtHook
	{
	public:
		explicit VmtHook(void* obj, std::size_t num_funcs);
		~VmtHook();

		VmtHook(VmtHook&& that) = delete;
		VmtHook& operator=(VmtHook&& that) = delete;
		VmtHook(VmtHook const&) = delete;
		VmtHook& operator=(VmtHook const&) = delete;

		void Hook(std::size_t index, void* func);
		void Unhook(std::size_t index);

		template<typename T>
		T GetOriginal(std::size_t index);

		void Enable();
		void Disable();

	private:
		void*** m_object;
		std::size_t m_num_funcs;

		void** m_original_table;
		std::unique_ptr<void* []> m_new_table;
	};

	template<typename T>
	inline T VmtHook::GetOriginal(std::size_t index)
	{
		return static_cast<T>(m_original_table[index]);
	}
}
