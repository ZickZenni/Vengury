#pragma once

#include <commands/command.hpp>

namespace Vengury {
	class VehSpawnCommand : public Command {
	public:
		VehSpawnCommand();

		bool Execute(PlayerPtr sender, std::vector<std::string> args);
	};
}