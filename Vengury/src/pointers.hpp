#pragma once

#include "common.hpp"

#include <player/CPlayerInfo.hpp>
#include <network/Network.hpp>
#include <network/ChatData.hpp>
#include <socialclub/FriendRegistry.hpp>

#include "gta/scrNative.hpp"
#include "function_types.hpp"

namespace Vengury
{
	class Pointers
	{
	public:
		explicit Pointers();
		~Pointers();
	public:
		HWND m_hwnd{};

		eGameState *m_game_state{};
		bool *m_is_session_started{};

		CPedFactory **m_ped_factory{};
		CNetworkPlayerMgr **m_network_player_mgr{};
		
		Network** m_network;

		rage::scrNativeRegistrationTable *m_native_registration_table{};
		functions::get_native_handler_t m_get_native_handler{};
		functions::fix_vectors_t m_fix_vectors{};

		rage::atArray<GtaThread*> *m_script_threads{};
		rage::scrProgramTable *m_script_program_table{};
		functions::run_script_threads_t m_run_script_threads{};
		std::int64_t **m_script_globals{};

		CGameScriptHandlerMgr **m_script_handler_mgr{};

		IDXGISwapChain **m_swapchain{};

		PVOID m_model_spawn_bypass;
		PVOID m_receive_net_message;

		functions::handle_to_ptr m_handle_to_ptr{};
		functions::request_control m_request_control{};
		functions::handle_remove_gamer_cmd m_handle_remove_gamer_cmd;
		functions::request_ragdoll m_request_ragdoll;

		functions::queue_packet m_queue_packet;
		functions::get_connection_peer m_get_connection_peer;
		functions::send_packet m_send_packet;

		ChatData** m_chat_data;

		// Bitbuffer Read/Write START
		functions::read_bitbuf_dword m_read_bitbuf_dword;
		functions::read_bitbuf_string m_read_bitbuf_string;
		functions::read_bitbuf_bool m_read_bitbuf_bool;
		functions::read_bitbuf_array m_read_bitbuf_array;
		functions::write_bitbuf_qword m_write_bitbuf_qword;
		functions::write_bitbuf_dword m_write_bitbuf_dword;
		functions::write_bitbuf_int64 m_write_bitbuf_int64;
		functions::write_bitbuf_int32 m_write_bitbuf_int32;
		functions::write_bitbuf_bool m_write_bitbuf_bool;
		functions::write_bitbuf_array m_write_bitbuf_array;
		// Bitbuffer Read/Write END

		PVOID m_assign_physical_index;

		FriendRegistry* m_friend_registry;

	};

	inline Pointers *g_pointers{};
}
