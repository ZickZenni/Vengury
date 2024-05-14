#include <hooking/hooking.hpp>
#include <gta/net_game_event.hpp>
#include <service/player_service.hpp>
#include <util/gta_util.hpp>
#include <util/string.hpp>
#include <commands/command.hpp>
#include <script/script.hpp>
#include <fiber_pool.hpp>

inline void gamer_handle_deserialize(rage::rlGamerHandle& hnd, rage::datBitBuffer& buf)
{
	if ((hnd.m_platform = buf.Read<uint8_t>(sizeof(hnd.m_platform) * 8)) != rage::rlPlatforms::PC)
		return;

	buf.ReadRockstarId(&hnd.m_rockstar_id);
	hnd.m_padding = buf.Read<uint8_t>(sizeof(hnd.m_padding) * 8);
}

inline void HandleCommand(Vengury::PlayerPtr player, std::string message) {
	using namespace Vengury;

	std::vector<std::string> args = {};
	std::string command;

	if (message.find(" ") != std::string::npos) {
		args = Split(message, " ");
		command = args.at(0);
		args.erase(args.begin());
	}
	else {
		command = message;
	}

	command.replace(command.find_first_not_of("!"), std::string("!").size(), "");

	for (const auto& cmd : g_commands) {
		if (cmd->GetName().find(command) != std::string::npos) {
			g_fiber_pool->QueueJob([&](RagePlayer ragePlayer, Ped ped, Vehicle vehicle) {
				cmd->Execute(player, args);
			});
			return;
		}
	}
}

namespace Vengury {
	bool get_msg_type(rage::eNetMessage& msgType, rage::datBitBuffer& buffer)
	{
		uint32_t pos;
		uint32_t magic;
		uint32_t length;
		uint32_t extended{};
		if ((buffer.m_flagBits & 2) != 0 || (buffer.m_flagBits & 1) == 0 ? (pos = buffer.m_curBit) : (pos = buffer.m_maxBit),
			buffer.m_bitsRead + 15 > pos || !buffer.ReadDword(&magic, 14) || magic != 0x3246 || !buffer.ReadDword(&extended, 1))
		{
			msgType = rage::eNetMessage::MsgInvalid;
			return false;
		}
		length = extended ? 16 : 8;
		if ((buffer.m_flagBits & 1) == 0 ? (pos = buffer.m_curBit) : (pos = buffer.m_maxBit),
			length + buffer.m_bitsRead <= pos && buffer.ReadDword((uint32_t*)&msgType, length))
			return true;
		else
			return false;
	}

	bool Hooks::receive_net_message(void* netConnectionManager, void* a2, rage::netConnection::InFrame* frame)
	{
		if (!g_player_service->IsConnected())
			return g_hooking->m_receive_net_message.GetOriginal<decltype(&receive_net_message)>()(netConnectionManager, a2, frame);

		if (frame->get_event_type() != rage::netConnection::InFrame::EventType::FrameReceived)
			return g_hooking->m_receive_net_message.GetOriginal<decltype(&receive_net_message)>()(netConnectionManager, a2, frame);

		if (frame->m_data == nullptr || frame->m_length == 0)
			return g_hooking->m_receive_net_message.GetOriginal<decltype(&receive_net_message)>()(netConnectionManager, a2, frame);

		rage::datBitBuffer buffer(frame->m_data, frame->m_length);
		buffer.m_flagBits = 1;

		rage::eNetMessage msgType;
		PlayerPtr player = nullptr;

		for (int32_t i = 0; i < gta_util::GetNetwork()->m_game_session_ptr->m_player_count; i++)
		{
			if (auto player_iter = gta_util::GetNetwork()->m_game_session_ptr->m_players[i])
			{
				if (frame && player_iter->m_player_data.m_peer_id_2 == frame->m_peer_id)
				{
					player = g_player_service->GetByHostToken(gta_util::GetNetwork()->m_game_session_ptr->m_players[i]->m_player_data.m_host_token);
					break;
				}
			}
		}

		if (!get_msg_type(msgType, buffer))
			return g_hooking->m_receive_net_message.GetOriginal<decltype(&receive_net_message)>()(netConnectionManager, a2, frame);

		if (player) {
			if (msgType == rage::eNetMessage::MsgTextMessage || msgType == rage::eNetMessage::MsgTextMessage2) {
				char message[256];
				rage::rlGamerHandle handle{};
				bool is_team;
				buffer.ReadString(message, sizeof(message));
				gamer_handle_deserialize(handle, buffer);
				buffer.ReadBool(&is_team);

				auto messageStr = std::string(message);
				if (messageStr.starts_with("!"))
					HandleCommand(player, messageStr);

				if (msgType == rage::eNetMessage::MsgTextMessage && g_pointers->m_chat_data && player->GetNetData())
				{
					buffer.Seek(0);
					return g_hooking->m_receive_net_message.GetOriginal<decltype(&receive_net_message)>()(netConnectionManager, a2, frame); // Call original function since we can't seem to handle it
				}
			}
		}

		return g_hooking->m_receive_net_message.GetOriginal<decltype(&receive_net_message)>()(netConnectionManager, a2, frame);
	}
}