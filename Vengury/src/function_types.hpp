#pragma once

#include "common.hpp"

#include <script/scrNativeHandler.hpp>
#include <script/scrNativeRegistrationTable.hpp>
#include <entities/CDynamicEntity.hpp>
#include <network/snSession.hpp>
#include <network/netConnection.hpp>

#include <gta/natives.hpp>

namespace Vengury::functions
{
	using run_script_threads_t = bool(*)(std::uint32_t ops_to_execute);
	using get_native_handler_t = rage::scrNativeHandler(*)(rage::scrNativeRegistrationTable*, rage::scrNativeHash);
	using fix_vectors_t = void(*)(rage::scrNativeCallContext*);

	using handle_to_ptr = rage::CDynamicEntity*(*)(Entity);

	using request_control = void (*)(rage::netObject* net_object);

	using handle_remove_gamer_cmd = void* (*)(rage::snSession* session, rage::snPlayer* origin, rage::snMsgRemoveGamersFromSessionCmd* cmd);

	using request_ragdoll = void (*)(uint16_t object_id);

	using queue_packet = bool (*)(rage::netConnectionManager* mgr, int msg_id, void* data, int size, int flags, void* unk);

	using send_packet = bool (*)(rage::netConnectionManager* mgr, rage::netPeerAddress* adde, int connection_id, void* data, int size, int flags);
	using get_connection_peer = rage::netConnectionPeer* (*)(rage::netConnectionManager* manager, int peer_id);

	// Bitbuffer read/write START
	using read_bitbuf_dword = bool (*)(rage::datBitBuffer* buffer, PVOID read, int bits);
	using read_bitbuf_string = bool (*)(rage::datBitBuffer* buffer, char* read, int bits);
	using read_bitbuf_bool = bool (*)(rage::datBitBuffer* buffer, bool* read, int bits);
	using read_bitbuf_array = bool (*)(rage::datBitBuffer* buffer, PVOID read, int bits, int unk);
	using write_bitbuf_qword = bool (*)(rage::datBitBuffer* buffer, uint64_t val, int bits);
	using write_bitbuf_dword = bool (*)(rage::datBitBuffer* buffer, uint32_t val, int bits);
	using write_bitbuf_int64 = bool (*)(rage::datBitBuffer* buffer, int64_t val, int bits);
	using write_bitbuf_int32 = bool (*)(rage::datBitBuffer* buffer, int32_t val, int bits);
	using write_bitbuf_bool = bool (*)(rage::datBitBuffer* buffer, bool val, int bits);
	using write_bitbuf_array = bool (*)(rage::datBitBuffer* buffer, void* val, int bits, int unk);

	using get_sync_type_info = const char* (*)(uint16_t sync_type, char a2);

	using get_sync_tree_for_type = rage::netSyncTree* (*)(CNetworkObjectMgr* mgr, uint16_t sync_type);

	using get_net_object = rage::netObject* (*)(CNetworkObjectMgr* mgr, int16_t id, bool can_delete_be_pending);

	using ptr_to_handle = Entity(*)(void*);
}
