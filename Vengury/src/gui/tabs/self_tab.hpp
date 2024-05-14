#pragma once

#include "../tab.hpp"

namespace Vengury
{
	class SelfTab : public BaseTab
	{
	public:
		void Render();

		void Tick();

	private:
		void RenderTopbar();

	private:
		bool m_GodMode = false;
		bool m_NeverWanted = false;
		bool m_SuperJump = false;
		bool m_FastRun = false;
		bool m_NoRagdoll = false;
	};
}