#pragma once

#include "imgui.h"

#include <format>

namespace Vengury {
	class Views {
	public:
		static void RenderOverlay();

		static void RenderMenu();

		static void RenderNotification();
	};
}