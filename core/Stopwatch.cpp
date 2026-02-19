#include "pch.h"
#include "Stopwatch.h"

namespace badCore
{
	Stopwatch::Stopwatch()noexcept
		:mWatch(std::chrono::steady_clock::now())
	{
	}

	std::chrono::steady_clock::duration Stopwatch::elapsed()noexcept {
		const auto old = mWatch;
		mWatch = std::chrono::steady_clock::now();
		return mWatch - old;
	}

	double Stopwatch::dt_float()noexcept
	{
		return std::chrono::duration_cast<Valuesec>(elapsed()).count();
	}

	std::size_t Stopwatch::dt_millisec() noexcept
	{
		return std::chrono::duration_cast<Millisec>(elapsed()).count();
	}

	std::size_t Stopwatch::dt_microsec() noexcept
	{
		return std::chrono::duration_cast<Microsec>(elapsed()).count();
	}

	std::size_t Stopwatch::dt_nanosec() noexcept
	{
		return std::chrono::duration_cast<Nanosec>(elapsed()).count();
	}

	void Stopwatch::reset()noexcept
	{
		mWatch = std::chrono::steady_clock::now();
	}
}