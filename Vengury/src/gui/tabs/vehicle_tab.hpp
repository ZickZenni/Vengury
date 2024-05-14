#pragma once

#include "../tab.hpp"

namespace Vengury
{
	class VehicleTab : public BaseTab
	{
	public:
		void Render();

		void Tick();

	private:
		void RenderTopbar() const;
		void RenderSpawningMenu();

		void SpawnVehicle(Hash hash, Vector3 pos);

	private:
		bool m_Godmode = false;
		bool m_HonkBoosting = false;
		bool m_RainbowCar = false;
		bool m_Invisible = false;

		bool m_SpawningGodMode = false;
		bool m_SpawningInvisible = false;

		char m_SpawningInput[128] {};

		// States
		bool m_InVehicle = false;
	};
}