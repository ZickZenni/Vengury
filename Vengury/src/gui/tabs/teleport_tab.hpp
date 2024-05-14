#pragma once

#include "../tab.hpp"

namespace Vengury
{
	class TeleportTab : public BaseTab
	{
	public:
		void Render();

	private:
		static void TeleportToBlip(Ped ped, int blipId);
	};
}