#pragma once

#include <imgui.h>

#include "common.hpp"

namespace Vengury
{
	class Renderer
	{
	public:
		explicit Renderer();
		~Renderer();

		void OnPresent();

		void PreReset();
		void PostReset();

		void WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	public:
		ImFont *m_font;
		ImFont *m_monospace_font;
	private:
		comptr<IDXGISwapChain> m_dxgi_swapchain;
		comptr<ID3D11Device> m_d3d_device;
		comptr<ID3D11DeviceContext> m_d3d_device_context;
	};

	inline Renderer *g_renderer{};
}
