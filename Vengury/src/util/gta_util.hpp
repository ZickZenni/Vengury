#pragma once
#include "common.hpp"
#include "ped/CPedFactory.hpp"
#include <script/GtaThread.hpp>
#include "pointers.hpp"
#include <network/CNetworkPlayerMgr.hpp>

namespace Vengury::gta_util
{
	inline CPed *GetLocalPed()
	{
		if (auto ped_factory = *g_pointers->m_ped_factory)
		{
			return ped_factory->m_local_ped;
		}

		return nullptr;
	}

	inline CPlayerInfo *GetLocalPlayerInfo()
	{
		if (auto ped_factory = *g_pointers->m_ped_factory)
		{
			if (auto ped = ped_factory->m_local_ped)
			{
				return ped->m_player_info;
			}
		}

		return nullptr;
	}

	inline CNetworkPlayerMgr* GetNetworkPlayerManager()
	{
		return *g_pointers->m_network_player_mgr;
	}

	inline Network* GetNetwork() {
		return *g_pointers->m_network;
	}

	template <typename F, typename ...Args>
	void ExecuteAsScript(rage::joaat_t script_hash, F &&callback, Args &&...args)
	{
		auto tls_ctx = rage::tlsContext::get();
		for (auto thread : *g_pointers->m_script_threads)
		{
			if (!thread || !thread->m_context.m_thread_id || thread->m_context.m_script_hash != script_hash)
				continue;

			auto og_thread = tls_ctx->m_script_thread;

			tls_ctx->m_script_thread = thread;
			tls_ctx->m_is_script_thread_active = true;

			std::invoke(std::forward<F>(callback), std::forward<Args>(args)...);

			tls_ctx->m_script_thread = og_thread;
			tls_ctx->m_is_script_thread_active = og_thread != nullptr;

			return;
		}
	}
}
