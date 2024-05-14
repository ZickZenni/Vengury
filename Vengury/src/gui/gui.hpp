#pragma once
#include "common.hpp"
#include "tab.hpp"

namespace Vengury
{
	class Gui
	{
	public:
		void InitRender();
		void Render();

		void InitScript();
		void TickScript();
		static void RunScript();

	public:
		bool m_opened{};
		uint32_t m_currentTab = 0;

		std::vector<Tab> m_tabs{};
	};

	inline Gui g_gui;
}
