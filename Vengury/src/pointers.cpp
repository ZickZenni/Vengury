#include "common.hpp"
#include "logger.hpp"
#include "pointers.hpp"
#include "memory/all.hpp"

namespace Vengury
{
	Pointers::Pointers()
	{
		memory::PatternBatch main_batch;

		main_batch.Add("Game state", "83 3D ? ? ? ? ? 75 17 8B 43 20 25", [this](memory::Handle ptr)
			{
				m_game_state = ptr.add(2).rip().as<eGameState*>();
			});

		main_batch.Add("Is session started", "40 38 35 ? ? ? ? 75 0E 4C 8B C3 49 8B D7 49 8B CE", [this](memory::Handle ptr)
			{
				m_is_session_started = ptr.add(3).rip().as<bool*>();
			});

		main_batch.Add("Ped factory", "48 8B 05 ? ? ? ? 48 8B 48 08 48 85 C9 74 52 8B 81", [this](memory::Handle ptr)
			{
				m_ped_factory = ptr.add(3).rip().as<CPedFactory**>();
			});

		main_batch.Add("Network player manager", "48 8B 0D ? ? ? ? 8A D3 48 8B 01 FF 50 ? 4C 8B 07 48 8B CF", [this](memory::Handle ptr)
			{
				m_network_player_mgr = ptr.add(3).rip().as<CNetworkPlayerMgr**>();
			});

		main_batch.Add("Native handlers", "48 8D 0D ? ? ? ? 48 8B 14 FA E8 ? ? ? ? 48 85 C0 75 0A", [this](memory::Handle ptr)
			{
				m_native_registration_table = ptr.add(3).rip().as<rage::scrNativeRegistrationTable*>();
				m_get_native_handler = ptr.add(12).rip().as<functions::get_native_handler_t>();
			});

		main_batch.Add("Fix vectors", "83 79 18 00 48 8B D1 74 4A FF 4A 18 48 63 4A 18 48 8D 41 04 48 8B 4C CA", [this](memory::Handle ptr)
			{
				m_fix_vectors = ptr.as<functions::fix_vectors_t>();
			});

		main_batch.Add("Script threads", "45 33 F6 8B E9 85 C9 B8", [this](memory::Handle ptr)
			{
				m_script_threads = ptr.sub(4).rip().sub(8).as<decltype(m_script_threads)>();
				m_run_script_threads = ptr.sub(0x1F).as<functions::run_script_threads_t>();
			});

		main_batch.Add("Script programs", "44 8B 0D ? ? ? ? 4C 8B 1D ? ? ? ? 48 8B 1D ? ? ? ? 41 83 F8 FF 74 3F 49 63 C0 42 0F B6 0C 18 81 E1", [this](memory::Handle ptr)
			{
				m_script_program_table = ptr.add(17).rip().as<decltype(m_script_program_table)>();
			});

		main_batch.Add("Script globals", "48 8D 15 ? ? ? ? 4C 8B C0 E8 ? ? ? ? 48 85 FF 48 89 1D", [this](memory::Handle ptr)
			{
				m_script_globals = ptr.add(3).rip().as<std::int64_t**>();
			});

		main_batch.Add("CGameScriptHandlerMgr", "48 8B 0D ? ? ? ? 4C 8B CE E8 ? ? ? ? 48 85 C0 74 05 40 32 FF", [this](memory::Handle ptr)
			{
				m_script_handler_mgr = ptr.add(3).rip().as<CGameScriptHandlerMgr**>();
			});

		main_batch.Add("Swapchain", "48 8B 0D ? ? ? ? 48 8B 01 44 8D 43 01 33 D2 FF 50 40 8B C8", [this](memory::Handle ptr)
			{
				m_swapchain = ptr.add(3).rip().as<IDXGISwapChain**>();
			});

		main_batch.Add("Model Spawn Bypass", "48 8B C8 FF 52 30 84 C0 74 05 48", [this](memory::Handle ptr)
			{
				m_model_spawn_bypass = ptr.add(8).as<PVOID>();
			});

		main_batch.Add("Handle To Ptr", "83 F9 FF 74 31 4C 8B 0D", [this](memory::Handle ptr)
			{
				m_handle_to_ptr = ptr.as<decltype(m_handle_to_ptr)>();
			});

		main_batch.Add("Request Control", "E8 ? ? ? ? EB 3E 48 8B D3", [this](memory::Handle ptr)
			{
				m_request_control = ptr.add(1).rip().as<decltype(m_request_control)>();
			});
		main_batch.Add("Network", "48 8B 0D ? ? ? ? 48 8B D7 E8 ? ? ? ? 84 C0 75 17 48 8B 0D ? ? ? ? 48 8B D7", [this](memory::Handle ptr)
			{
				m_network = ptr.add(3).rip().as<Network**>();
			});
		main_batch.Add("Handle Remove Gamer Command", "48 85 D2 0F 84 0E 04", [this](memory::Handle ptr)
			{
				m_handle_remove_gamer_cmd = ptr.as<functions::handle_remove_gamer_cmd>();
			});
		main_batch.Add("Receive Net Message", "48 83 EC 20 4C 8B 71 50 33 ED", [this](memory::Handle ptr)
			{
				m_receive_net_message = ptr.sub(0x19).as<PVOID>();
			});
		main_batch.Add("Chat Data", "48 8B 05 ? ? ? ? 0F 45 DF", [this](memory::Handle ptr)
			{
				m_chat_data = ptr.add(3).rip().as<ChatData**>();
			});


		main_batch.Add("Read Bitbuffer WORD/DWORD", "48 89 74 24 ? 57 48 83 EC 20 48 8B D9 33 C9 41 8B F0 8A", [this](memory::Handle ptr)
			{
				m_read_bitbuf_dword = ptr.sub(5).as<decltype(m_read_bitbuf_dword)>();
			});
		main_batch.Add("Read Bitbuffer Array", "48 89 5C 24 ? 57 48 83 EC 30 41 8B F8 4C", [this](memory::Handle ptr)
			{
				m_read_bitbuf_array = ptr.as<decltype(m_read_bitbuf_array)>();
			});
		main_batch.Add("Read Bitbuffer String", "48 89 5C 24 08 48 89 6C 24 18 56 57 41 56 48 83 EC 20 48 8B F2 45", [this](memory::Handle ptr)
			{
				m_read_bitbuf_string = ptr.as<decltype(m_read_bitbuf_string)>();
			});
		main_batch.Add("Read Bitbuffer Boolean", "48 8B C4 48 89 58 08 55 56 57 48 83 EC 20 48 83 60", [this](memory::Handle ptr)
			{
				m_read_bitbuf_bool = ptr.as<decltype(m_read_bitbuf_bool)>();
			});
		main_batch.Add("Write Bitbuffer WORD/DWORD", "48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 48 89 78 20 41 56 48 83 EC 20 8B EA BF 01", [this](memory::Handle ptr)
			{
				m_write_bitbuf_dword = ptr.as<decltype(m_write_bitbuf_dword)>();
			});
		main_batch.Add("Write Bitbuffer QWORD", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 20 41 8B F0 48 8B EA 48 8B D9 41 83 F8 20", [this](memory::Handle ptr)
			{
				m_write_bitbuf_qword = ptr.as<decltype(m_write_bitbuf_qword)>();
			});
		main_batch.Add("Write Bitbuffer Int64", "E8 ? ? ? ? 8A 53 39 48 8B CF", [this](memory::Handle ptr)
			{
				m_write_bitbuf_int64 = ptr.add(1).rip().as<decltype(m_write_bitbuf_int64)>();
			});
		main_batch.Add("Write Bitbuffer Int32", "E8 ? ? ? ? 8A 53 74", [this](memory::Handle ptr)
			{
				m_write_bitbuf_int32 = ptr.add(1).rip().as<decltype(m_write_bitbuf_int32)>();
			});
		main_batch.Add("Write Bitbuffer Boolean", "E8 ? ? ? ? 8A 57 39", [this](memory::Handle ptr)
			{
				m_write_bitbuf_bool = ptr.add(1).rip().as<decltype(m_write_bitbuf_bool)>();
			});
		main_batch.Add("Write Bitbuffer Array", "E8 ? ? ? ? 01 7E 08", [this](memory::Handle ptr)
			{
				m_write_bitbuf_array = ptr.add(1).rip().as<decltype(m_write_bitbuf_array)>();
			});

		main_batch.Add("Request Ragdoll", "E8 ? ? ? ? 09 B3 ? ? ? ? 48 8B 5C 24",
			[this](memory::Handle ptr)
			{
				m_request_ragdoll = ptr.add(1).rip().as<functions::request_ragdoll>();
			});		
		main_batch.Add("Queue Packet", "E8 ? ? ? ? 84 C0 74 4D B3 01",
			[this](memory::Handle ptr)
			{
				m_queue_packet = ptr.add(1).rip().as<functions::queue_packet>();
			});		
		
		main_batch.Add("Get Connection Peer", "48 89 5C 24 08 48 89 74 24 18 89 54 24 10 57 48 83 EC 40 48",
			[this](memory::Handle ptr)
			{
				m_get_connection_peer = ptr.as<functions::get_connection_peer>();
			});	

		main_batch.Add("Send Packet", "48 8B C4 48 89 58 08 48 89 70 10 48 89 78 18 4C 89 48 20 55 41 54 41 55 41 56 41 57 48 8D A8 98",
			[this](memory::Handle ptr)
			{
				m_send_packet = ptr.as<functions::send_packet>();
			});

		main_batch.Add("Assign Physical Index", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 54 41 55 41 56 41 57 48 83 EC 20 41 8A E8",
			[this](memory::Handle ptr)
			{
				m_assign_physical_index = ptr.as<PVOID>();
			});

		main_batch.Add("Friend Registry", "3B 0D ? ? ? ? 73 17",
			[this](memory::Handle ptr)
			{
				m_friend_registry = ptr.add(2).rip().as<FriendRegistry*>();
			});

		main_batch.Run(memory::Module(nullptr));

		m_hwnd = FindWindowW(L"grcWindow", nullptr);
		if (!m_hwnd)
			throw std::runtime_error("Failed to find the game's window.");

		g_pointers = this;
	}

	Pointers::~Pointers()
	{
		g_pointers = nullptr;
	}
}
