/*#include "common.hpp"
#include "function_types.hpp"
#include "logger.hpp"
#include <rage/atArray.hpp>
#include <script/GtaThread.hpp>
#include <script/scrThread.hpp>
#include "Gui/Gui.hpp"
#include "Hooking.hpp"
#include "memory/module.hpp"
#include "gta/natives.hpp"
#include "pointers.hpp"
#include "renderer.hpp"
#include "ScriptManager.hpp"

#include <MinHook.h>

namespace Vengury
{
	static GtaThread* find_script_thread(rage::joaat_t hash)
	{
		for (auto thread : *g_pointers->m_script_threads)
		{
			if (thread
				&& thread->m_context.m_thread_id
				&& thread->m_handler
				&& thread->m_script_hash == hash)
			{
				return thread;
			}
		}

		return nullptr;
	}

	Hooking::Hooking() :
		m_swapchain_hook(*g_pointers->m_swapchain, hooks::swapchain_num_funcs),
		m_sync_data_reader_hook(g_pointers->m_sync_data_reader_vtable, 27)
	{
		m_swapchain_hook.hook(hooks::swapchain_present_index, &hooks::swapchain_present);
		m_swapchain_hook.hook(hooks::swapchain_resizebuffers_index, &hooks::swapchain_resizebuffers);

		m_sync_data_reader_hook.hook(1, &hooks::sync_reader_serialize_dword);
		m_sync_data_reader_hook.hook(2, &hooks::sync_reader_serialize_word);
		m_sync_data_reader_hook.hook(3, &hooks::sync_reader_serialize_byte);
		m_sync_data_reader_hook.hook(4, &hooks::sync_reader_serialize_int32);
		m_sync_data_reader_hook.hook(5, &hooks::sync_reader_serialize_int16);
		m_sync_data_reader_hook.hook(6, &hooks::sync_reader_serialize_signed_byte);
		m_sync_data_reader_hook.hook(7, &hooks::sync_reader_serialize_bool);
		m_sync_data_reader_hook.hook(9, &hooks::sync_reader_serialize_int32);
		m_sync_data_reader_hook.hook(10, &hooks::sync_reader_serialize_int16);
		m_sync_data_reader_hook.hook(11, &hooks::sync_reader_serialize_signed_byte);
		m_sync_data_reader_hook.hook(13, &hooks::sync_reader_serialize_dword);
		m_sync_data_reader_hook.hook(14, &hooks::sync_reader_serialize_word);
		m_sync_data_reader_hook.hook(15, &hooks::sync_reader_serialize_byte);
		m_sync_data_reader_hook.hook(16, &hooks::sync_reader_serialize_signed_float);
		m_sync_data_reader_hook.hook(17, &hooks::sync_reader_serialize_float);
		m_sync_data_reader_hook.hook(18, &hooks::sync_reader_serialize_net_id);
		m_sync_data_reader_hook.hook(19, &hooks::sync_reader_serialize_vec3);
		m_sync_data_reader_hook.hook(21, &hooks::sync_reader_serialize_vec3_signed);
		m_sync_data_reader_hook.hook(23, &hooks::sync_reader_serialize_array);

		// The only instances in that vector at this point should only be the "lazy" hooks
		// aka the ones that still don't have their m_target assigned
		for (auto& detour_hook_helper : m_detour_hook_helpers)
		{
			detour_hook_helper.m_detour_hook->set_target_and_create_hook(detour_hook_helper.m_on_hooking_available());
		}

		detour_hook_helper::add<hooks::run_script_threads>("SH", g_pointers->m_run_script_threads);
		detour_hook_helper::add<hooks::receive_net_message>("RNM", g_pointers->m_receive_net_message);

		g_hooking = this;
	}

	Hooking::~Hooking()
	{
		if (m_enabled)
			disable();

		g_hooking = nullptr;
	}

	void Hooking::enable()
	{
		m_swapchain_hook.enable();
		m_sync_data_reader_hook.enable();
		m_og_wndproc = WNDPROC(SetWindowLongPtrW(g_pointers->m_hwnd, GWLP_WNDPROC, LONG_PTR(&hooks::wndproc)));

		for (auto& detour_hook_helper : m_detour_hook_helpers)
		{
			detour_hook_helper.m_detour_hook->enable();
		}

		MH_ApplyQueued();

		m_enabled = true;
	}

	void Hooking::disable()
	{
		m_enabled = false;

		for (auto& detour_hook_helper : m_detour_hook_helpers)
		{
			detour_hook_helper.m_detour_hook->disable();
		}

		SetWindowLongPtrW(g_pointers->m_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(m_og_wndproc));
		m_sync_data_reader_hook.disable();
		m_swapchain_hook.disable();

		MH_ApplyQueued();

		m_detour_hook_helpers.clear();
	}

	minhook_keepalive::minhook_keepalive()
	{
		MH_Initialize();
	}

	minhook_keepalive::~minhook_keepalive()
	{
		MH_Uninitialize();
	}

	bool hooks::run_script_threads(std::uint32_t ops_to_execute)
	{
		if (g_running)
		{
			g_script_mgr.tick();
		}

		return g_hooking->get_original<run_script_threads>()(ops_to_execute);
	}

	HRESULT hooks::swapchain_present(IDXGISwapChain* this_, UINT sync_interval, UINT flags)
	{
		if (g_running && ((flags & (UINT)DXGI_PRESENT_TEST) != (UINT)DXGI_PRESENT_TEST))
		{
			g_renderer->on_present();
		}

		return g_hooking->m_swapchain_hook.get_original<decltype(&swapchain_present)>(swapchain_present_index)(this_, sync_interval, flags);
	}

	HRESULT hooks::swapchain_resizebuffers(IDXGISwapChain* this_, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT swapchain_flags)
	{
		if (g_running)
		{
			g_renderer->pre_reset();

			const auto result = g_hooking->m_swapchain_hook.get_original<decltype(&swapchain_resizebuffers)>(swapchain_resizebuffers_index)(this_, buffer_count, width, height, new_format, swapchain_flags);

			if (SUCCEEDED(result))
			{
				g_renderer->post_reset();
			}

			return result;
		}

		return g_hooking->m_swapchain_hook.get_original<decltype(&swapchain_resizebuffers)>(swapchain_resizebuffers_index)(this_, buffer_count, width, height, new_format, swapchain_flags);
	}

	LRESULT hooks::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		if (g_running)
		{
			g_renderer->wndproc(hwnd, msg, wparam, lparam);
		}

		return CallWindowProcW(g_hooking->m_og_wndproc, hwnd, msg, wparam, lparam);
	}

	void Hooking::detour_hook_helper::enable_hook_if_hooking_is_already_running()
	{
		if (g_hooking && g_hooking->m_enabled)
		{
			if (m_on_hooking_available)
			{
				m_detour_hook->set_target_and_create_hook(m_on_hooking_available());
			}

			m_detour_hook->enable();
			MH_ApplyQueued();
		}
	}

	Hooking::detour_hook_helper::~detour_hook_helper()
	{
	}
}
*/

#include "function_types.hpp"
#include "logger.hpp"
#include <rage/atArray.hpp>
#include <script/GtaThread.hpp>
#include <script/scrThread.hpp>
#include "gui/gui.hpp"
#include "hooking.hpp"
#include "memory/module.hpp"
#include "gta/natives.hpp"
#include "pointers.hpp"
#include "renderer.hpp"
#include "script/script_mgr.hpp"

#include <MinHook.h>

namespace Vengury
{
	static GtaThread* find_script_thread(rage::joaat_t hash)
	{
		for (auto thread : *g_pointers->m_script_threads)
		{
			if (thread
				&& thread->m_context.m_thread_id
				&& thread->m_handler
				&& thread->m_script_hash == hash)
			{
				return thread;
			}
		}

		return nullptr;
	}

	Hooking::Hooking() :
		m_swapchain_hook(*g_pointers->m_swapchain, Hooks::swapchain_num_funcs),
		m_set_cursor_pos_hook("SetCursorPos", memory::Module("user32.dll").GetExport("SetCursorPos").as<void*>(), &Hooks::set_cursor_pos),

		m_run_script_threads_hook("Script hook", g_pointers->m_run_script_threads, &Hooks::run_script_threads),

		m_receive_net_message("Receive Net Message", g_pointers->m_receive_net_message, &Hooks::receive_net_message),
		m_assign_physical_index("Receive Net Message", g_pointers->m_assign_physical_index, &Hooks::assign_physical_index),

		m_convert_thread_to_fiber_hook("ConvertThreadToFiber", memory::Module("kernel32.dll").GetExport("ConvertThreadToFiber").as<void*>(), &Hooks::convert_thread_to_fiber)

	{
		m_swapchain_hook.Hook(Hooks::swapchain_present_index, &Hooks::swapchain_present);
		m_swapchain_hook.Hook(Hooks::swapchain_resizebuffers_index, &Hooks::swapchain_resizebuffers);

		g_hooking = this;
	}

	Hooking::~Hooking()
	{
		if (m_enabled)
			Disable();

		g_hooking = nullptr;
	}

	void Hooking::Enable()
	{
		m_swapchain_hook.Enable();
		m_og_wndproc = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(g_pointers->m_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Hooks::wndproc)));
		m_set_cursor_pos_hook.Enable();

		m_run_script_threads_hook.Enable();
		m_convert_thread_to_fiber_hook.Enable();

		m_receive_net_message.Enable();
		m_assign_physical_index.Enable();

		m_enabled = true;
	}

	void Hooking::Disable()
	{
		m_enabled = false;

		m_receive_net_message.Disable();
		m_assign_physical_index.Disable();

		m_convert_thread_to_fiber_hook.Disable();
		m_run_script_threads_hook.Disable();

		m_set_cursor_pos_hook.Disable();
		SetWindowLongPtrW(g_pointers->m_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(m_og_wndproc));
		m_swapchain_hook.Disable();
	}

	MinhookKeepAlive::MinhookKeepAlive()
	{
		MH_Initialize();
	}

	MinhookKeepAlive::~MinhookKeepAlive()
	{
		MH_Uninitialize();
	}

	bool Hooks::run_script_threads(std::uint32_t ops_to_execute)
	{
		TRY_CLAUSE
		{
			if (g_running)
			{
				g_script_mgr.Tick();
			}

			return g_hooking->m_run_script_threads_hook.GetOriginal<functions::run_script_threads_t>()(ops_to_execute);
		} EXCEPT_CLAUSE
			return false;
	}

	void* Hooks::convert_thread_to_fiber(void* param)
	{
		TRY_CLAUSE
		{
			if (IsThreadAFiber())
			{
				return GetCurrentFiber();
			}

			return g_hooking->m_convert_thread_to_fiber_hook.GetOriginal<decltype(&convert_thread_to_fiber)>()(param);
		} EXCEPT_CLAUSE
			return nullptr;
	}

	HRESULT Hooks::swapchain_present(IDXGISwapChain* this_, UINT sync_interval, UINT flags)
	{
		TRY_CLAUSE
		{
			if (g_running)
			{
				g_renderer->OnPresent();
			}

			return g_hooking->m_swapchain_hook.GetOriginal<decltype(&swapchain_present)>(swapchain_present_index)(this_, sync_interval, flags);
		} EXCEPT_CLAUSE
			return NULL;
	}

	HRESULT Hooks::swapchain_resizebuffers(IDXGISwapChain* this_, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT swapchain_flags)
	{
		TRY_CLAUSE
		{
			if (g_running)
			{
				g_renderer->PreReset();

				auto result = g_hooking->m_swapchain_hook.GetOriginal<decltype(&swapchain_resizebuffers)>(swapchain_resizebuffers_index)
					(this_, buffer_count, width, height, new_format, swapchain_flags);

				if (SUCCEEDED(result))
				{
					g_renderer->PostReset();
				}

				return result;
			}

			return g_hooking->m_swapchain_hook.GetOriginal<decltype(&swapchain_resizebuffers)>(swapchain_resizebuffers_index)
				(this_, buffer_count, width, height, new_format, swapchain_flags);
		} EXCEPT_CLAUSE
			return NULL;
	}

	LRESULT Hooks::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		TRY_CLAUSE
		{
			if (g_running)
			{
				g_renderer->WndProc(hwnd, msg, wparam, lparam);
			}

			return CallWindowProcW(g_hooking->m_og_wndproc, hwnd, msg, wparam, lparam);
		} EXCEPT_CLAUSE
			return NULL;
	}

	BOOL Hooks::set_cursor_pos(int x, int y)
	{
		TRY_CLAUSE
		{
			if (g_gui.m_opened)
				return true;

			return g_hooking->m_set_cursor_pos_hook.GetOriginal<decltype(&set_cursor_pos)>()(x, y);
		} EXCEPT_CLAUSE
			return FALSE;
	}
}
