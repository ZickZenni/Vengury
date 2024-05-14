#pragma once
#include <vector>
#include "fwddec.hpp"
#include "handle.hpp"

namespace memory
{
	class Range
	{
	public:
		Range(Handle base, std::size_t size);

		Handle begin();
		Handle end();
		std::size_t size();

		bool Contains(Handle h);

		Handle Scan(Pattern const& sig);
		std::vector<Handle> ScanAll(Pattern const& sig);
	protected:
		Handle m_base;
		std::size_t m_size;
	};
}
