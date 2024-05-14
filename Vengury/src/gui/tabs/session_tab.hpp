#pragma once

#include "../tab.hpp"

namespace Vengury
{
	class SessionTab : public BaseTab
	{
	public:
		void Render();

		void Tick();
	private:
		void TickSelectedPeer(CNetworkPlayerMgr* mgr);

		void RenderPlayerList(CNetworkPlayerMgr* mgr);
		void RenderSelectedPlayer();

		bool m_InVehicle{};
	};
}