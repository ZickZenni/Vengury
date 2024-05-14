/*#pragma once
#include "common.hpp"
#include "Hooking/detour_hook.hpp"
#include "Hooking/script_hook.hpp"
#include "Hooking/vmt_hook.hpp"
#include <network/CNetGamePlayer.hpp>
#include <network/CNetworkPlayerMgr.hpp>
#include <network/netConnection.hpp>
#include <hooking/vtable_hook.hpp>

namespace Vengury
{
	class PlayerService;

	struct hooks
	{
		static bool run_script_threads(std::uint32_t ops_to_execute);

		static constexpr auto swapchain_num_funcs = 19;
		static constexpr auto swapchain_present_index = 8;
		static constexpr auto swapchain_resizebuffers_index = 13;
		static HRESULT swapchain_present(IDXGISwapChain *this_, UINT sync_interval, UINT flags);
		static HRESULT swapchain_resizebuffers(IDXGISwapChain *this_, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT swapchain_flags);

		//static void* assign_physical_index(CNetworkPlayerMgr* netPlayerMgr, CNetGamePlayer* player, uint8_t new_index);
		static bool receive_net_message(void* netConnectionManager, void* a2, rage::netConnection::InFrame* frame);

		static LRESULT wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

		static bool sync_reader_serialize_dword(void* _this, uint32_t* dword, int size);
		static bool sync_reader_serialize_word(void* _this, uint16_t* word, int size);
		static bool sync_reader_serialize_byte(void* _this, uint8_t* byte, int size);
		static bool sync_reader_serialize_int32(void* _this, int32_t* i, int size);
		static bool sync_reader_serialize_int16(void* _this, int16_t* i, int size);
		static bool sync_reader_serialize_signed_byte(void* _this, int8_t* i, int size);
		static bool sync_reader_serialize_bool(void* _this, bool* flag, int size);
		static bool sync_reader_serialize_signed_float(void* _this, float* flt, float divisor, int size);
		static bool sync_reader_serialize_float(void* _this, float* flt, float divisor, int size);
		static bool sync_reader_serialize_net_id(void* _this, uint16_t* id);
		static bool sync_reader_serialize_vec3(void* _this, rage::fvector3* vec, float divisor, int size);
		static bool sync_reader_serialize_vec3_signed(void* _this, rage::fvector3* vec, float divisor, int size);
		static bool sync_reader_serialize_array(void* _this, void* array, int size);
	};

	struct minhook_keepalive
	{
		minhook_keepalive();
		~minhook_keepalive();
	};

	class Hooking
	{
		friend hooks;
	public:
		explicit Hooking();
		~Hooking();

		void enable();
		void disable();

		class detour_hook_helper
		{
			friend Hooking;

			using ret_ptr_fn = std::function<void* ()>;

			ret_ptr_fn m_on_hooking_available = nullptr;

			detour_hook* m_detour_hook;

			void enable_hook_if_hooking_is_already_running();

			template<auto detour_function>
			struct hook_to_detour_hook_helper
			{
				static inline detour_hook m_detour_hook;
			};

		public:
			template<auto detour_function>
			static void add(const std::string& name, void* target)
			{
				hook_to_detour_hook_helper<detour_function>::m_detour_hook.set_instance(name, target, detour_function);

				detour_hook_helper d{};
				d.m_detour_hook = &hook_to_detour_hook_helper<detour_function>::m_detour_hook;

				d.enable_hook_if_hooking_is_already_running();

				m_detour_hook_helpers.push_back(d);
			}

			template<auto detour_function>
			static void* add_lazy(const std::string& name, detour_hook_helper::ret_ptr_fn on_hooking_available)
			{
				hook_to_detour_hook_helper<detour_function>::m_detour_hook.set_instance(name, detour_function);

				detour_hook_helper d{};
				d.m_detour_hook = &hook_to_detour_hook_helper<detour_function>::m_detour_hook;
				d.m_on_hooking_available = on_hooking_available;

				d.enable_hook_if_hooking_is_already_running();

				m_detour_hook_helpers.push_back(d);

				return nullptr;
			}

			~detour_hook_helper();
		};

		template<auto detour_function>
		static auto get_original()
		{
			return detour_hook_helper::hook_to_detour_hook_helper<detour_function>::m_detour_hook.get_original<decltype(detour_function)>();
		}

	private:
		bool m_enabled{};
		minhook_keepalive m_minhook_keepalive;

		vmt_hook m_swapchain_hook;
		vtable_hook m_sync_data_reader_hook;

		WNDPROC m_og_wndproc = nullptr;

		static inline std::vector<detour_hook_helper> m_detour_hook_helpers;
	};

	inline Hooking *g_hooking{};
}
*/

#pragma once
#include "common.hpp"
#include "hooking/detour_hook.hpp"
#include "hooking/script_hook.hpp"
#include "hooking/vmt_hook.hpp"
#include <network/CNetGamePlayer.hpp>
#include <network/CNetworkPlayerMgr.hpp>
#include <network/netConnection.hpp>
#include <hooking/vtable_hook.hpp>

namespace Vengury
{
	struct Hooks
	{
		static bool run_script_threads(std::uint32_t ops_to_execute);
		static void* convert_thread_to_fiber(void* param);

		static constexpr auto swapchain_num_funcs = 19;
		static constexpr auto swapchain_present_index = 8;
		static constexpr auto swapchain_resizebuffers_index = 13;
		static HRESULT swapchain_present(IDXGISwapChain* this_, UINT sync_interval, UINT flags);
		static HRESULT swapchain_resizebuffers(IDXGISwapChain* this_, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT swapchain_flags);

		static bool receive_net_message(void* netConnectionManager, void* a2, rage::netConnection::InFrame* frame);
		static void* assign_physical_index(CNetworkPlayerMgr* netPlayerMgr, CNetGamePlayer* player, uint8_t new_index);

		static LRESULT wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
		static BOOL set_cursor_pos(int x, int y);
	};

	struct MinhookKeepAlive
	{
		MinhookKeepAlive();
		~MinhookKeepAlive();
	};

	class Hooking
	{
		friend Hooks;
	public:
		explicit Hooking();
		~Hooking();

		void Enable();
		void Disable();

	private:
		bool m_enabled{};
		MinhookKeepAlive m_minhook_keepalive;

		VmtHook m_swapchain_hook;
		WNDPROC m_og_wndproc;
		DetourHook m_set_cursor_pos_hook;

		DetourHook m_run_script_threads_hook;
		DetourHook m_convert_thread_to_fiber_hook;
		DetourHook m_receive_net_message;
		DetourHook m_assign_physical_index;
	};

	inline Hooking* g_hooking{};
}
