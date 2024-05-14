#pragma once

#include "common.hpp"
#include "gta/natives.hpp"
#include "imgui.h"

namespace Vengury {
	class BaseTab {
	public:
		virtual void Render() = 0;
		virtual void Tick() {};
	};

	struct Tab {
		const char* name;
		BaseTab* tab;
	};
}