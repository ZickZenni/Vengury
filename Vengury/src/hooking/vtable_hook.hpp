#pragma once

namespace Vengury
{
	class VTableHook
	{
	public:
		explicit VTableHook(void** vft, std::size_t num_funcs);
		~VTableHook();

		VTableHook(VTableHook&& that) = delete;
		VTableHook& operator=(VTableHook&& that) = delete;
		VTableHook(VTableHook const&) = delete;
		VTableHook& operator=(VTableHook const&) = delete;

		void Hook(std::size_t index, void* func);
		void Unhook(std::size_t index);

		template<typename T>
		T GetOriginal(std::size_t index);

		inline void** GetOriginalTable()
		{
			return m_backup_table.get();
		}

		void Enable();
		void Disable();

	private:
		std::size_t m_num_funcs;
		void** m_table;
		std::unique_ptr<void* []> m_backup_table;
		std::unique_ptr<void* []> m_hook_table;
	};

	template<typename T>
	inline T VTableHook::GetOriginal(std::size_t index)
	{
		return static_cast<T>(m_backup_table[index]);
	}

}
