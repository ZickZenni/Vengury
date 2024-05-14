#include "../common.hpp"
#include "../logger.hpp"
#include "pattern_batch.hpp"
#include "range.hpp"

namespace memory
{
	void PatternBatch::Add(std::string name, Pattern pattern, std::function<void(Handle)> callback)
	{
		m_entries.emplace_back(std::move(name), std::move(pattern), std::move(callback));
	}

	void PatternBatch::Run(Range region)
	{
		bool all_found = true;
		for (auto& entry : m_entries)
		{
			if (auto result = region.Scan(entry.m_pattern))
			{
				if (entry.m_callback)
				{
					std::invoke(std::move(entry.m_callback), result);
					LOG(Vengury::INFO_TO_FILE) << "Found '" << entry.m_name << "' GTA5.exe+" << HEX_TO_UPPER(result.as<DWORD64>() - region.begin().as<DWORD64>());
				}
				else
				{
					all_found = false;
					LOG(WARNING) << "Failed to find '" << entry.m_name << "'.";
				}
			}
			else
			{
				all_found = false;
				LOG(WARNING) << "Failed to find '" << entry.m_name << "'.";
			}
		}

		m_entries.clear();
		if (!all_found)
		{
			throw std::runtime_error("Failed to find some patterns.");
		}
	}
}
