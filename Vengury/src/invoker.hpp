#pragma once

#include "common.hpp"

#include <script/scrNativeHandler.hpp>

namespace Vengury
{
	class NativeCallContext : public rage::scrNativeCallContext
	{
	public:
		NativeCallContext();
	private:
		std::uint64_t m_return_stack[10];
		std::uint64_t m_arg_stack[100];
	};

	class NativeInvoker
	{
	public:
		explicit NativeInvoker() = default;
		~NativeInvoker() = default;

		void CacheHandlers();

		void BeginCall();
		void EndCall(rage::scrNativeHash hash);

		template <typename T>
		void PushArg(T &&value)
		{
			m_call_context.push_arg(std::forward<T>(value));
		}

		template <typename T>
		T &GetReturnValue()
		{
			return *m_call_context.get_return_value<T>();
		}
	private:
		NativeCallContext m_call_context;
		std::unordered_map<rage::scrNativeHash, rage::scrNativeHandler> m_handler_cache;
	};

	inline NativeInvoker g_native_invoker;
}
