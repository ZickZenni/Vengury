#pragma once

#include <common.hpp>
#include <service/player_service.hpp>

namespace Vengury {
	class Command {
	public:
		explicit Command(std::string name) : m_Name(name) {}

		virtual bool Execute(PlayerPtr sender, std::vector<std::string> args) = 0;

		std::string GetName() const {
			return m_Name;
		}

	private:
		std::string m_Name;
	};

	inline std::vector<Command*> g_commands{};
}