#pragma once
#include <functional>
#include <vector>
#include "pattern.hpp"

namespace memory
{
	class PatternBatch
	{
	public:
		explicit PatternBatch() = default;
		~PatternBatch() noexcept = default;

		void Add(std::string name, Pattern pattern, std::function<void(memory::Handle)> callback);
		void Run(Range region);
	
		struct Entry
		{
			std::string m_name;
			Pattern m_pattern;
			std::function<void(memory::Handle)> m_callback;

			explicit Entry(std::string name, Pattern pattern, std::function<void(memory::Handle)> callback) :
				m_name(std::move(name)),
				m_pattern(std::move(pattern)),
				m_callback(std::move(callback))
			{}
		};
	
	private:
		std::vector<Entry> m_entries;
	};
}
