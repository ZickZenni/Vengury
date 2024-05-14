#pragma once

#include "common.hpp"

#include <gta/scrNative.hpp>

namespace Vengury
{
	class FiberPool
	{
	public:
		explicit FiberPool(std::size_t num_fibers);
		~FiberPool();

		void QueueJob(std::function<void(RagePlayer player, Ped ped, Vehicle vehicle)> func);

		void FiberTick();
		static void FiberFunc();
	private:
		std::recursive_mutex m_mutex;
		std::stack<std::function<void(RagePlayer player, Ped ped, Vehicle vehicle)>> m_jobs;
	};

	inline FiberPool *g_fiber_pool{};
}
