#include "time.hpp"

#include <chrono>

namespace Vengury {
	int64_t Time::GetMillis()
	{
		const auto duration = std::chrono::system_clock::now().time_since_epoch();
		return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	}
}