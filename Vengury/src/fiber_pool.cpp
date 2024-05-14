#include "fiber_pool.hpp"

#include "common.hpp"

#include "script/script.hpp"
#include "script/script_mgr.hpp"
#include "gta/natives.hpp"
#include "util/player_util.hpp"
#include "util/vehicle.hpp"

namespace Vengury
{
	FiberPool::FiberPool(std::size_t num_fibers)
	{
		for (std::size_t i = 0; i < num_fibers; ++i)
		{
			g_script_mgr.AddScript(std::make_unique<Script>(&FiberFunc));
		}

		g_fiber_pool = this;
	}

	FiberPool::~FiberPool()
	{
		g_fiber_pool = nullptr;
	}

	void FiberPool::QueueJob(std::function<void(RagePlayer player, Ped ped, Vehicle vehicle)> func)
	{
		if (func)
		{
			std::lock_guard lock(m_mutex);
			m_jobs.push(std::move(func));
		}
	}

	void FiberPool::FiberTick()
	{
		std::unique_lock lock(m_mutex);
		if (!m_jobs.empty())
		{
			auto job = std::move(m_jobs.top());
			m_jobs.pop();
			lock.unlock();

			const auto player = PLAYER::PLAYER_ID();

			if (!PLAYER::IS_PLAYER_PLAYING(player))
				return;

			Ped ped;

			if (!player::GetPlayerPed(player, &ped))
				return;

			Vehicle vehicle;

			vehicle::GetVehicle(ped, &vehicle);

			std::invoke(std::move(job), player, ped, vehicle);
		}
	}

	void FiberPool::FiberFunc()
	{
		while (true)
		{
			g_fiber_pool->FiberTick();
			Script::GetCurrent()->YieldScript();
		}
	}
}
