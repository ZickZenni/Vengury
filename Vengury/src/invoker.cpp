#include "invoker.hpp"

#include "common.hpp"

#include "logger.hpp"
#include "pointers.hpp"
#include "gta/crossmap.hpp"

namespace Vengury
{
	NativeCallContext::NativeCallContext()
	{
		m_return_value = &m_return_stack[0];
		m_args = &m_arg_stack[0];
	}

	void NativeInvoker::CacheHandlers()
	{
		for (const rage::scrNativeMapping& mapping : g_crossmap)
		{
			rage::scrNativeHandler handler = g_pointers->m_get_native_handler(
				g_pointers->m_native_registration_table, mapping.second);

			m_handler_cache.emplace(mapping.first, handler);
		}
	}

	void NativeInvoker::BeginCall()
	{
		m_call_context.reset();
	}

	void NativeInvoker::EndCall(rage::scrNativeHash hash)
	{
		if (auto it = m_handler_cache.find(hash); it != m_handler_cache.end())
		{
			rage::scrNativeHandler handler = it->second;

			handler(&m_call_context);
			g_pointers->m_fix_vectors(&m_call_context);
		}
		else
		{
			[hash]() { LOG(WARNING) << "Failed to find " << HEX_TO_UPPER(hash) << " native's handler."; }();
		}
	}
}
