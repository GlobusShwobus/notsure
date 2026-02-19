#pragma once

#include <chrono>

namespace badCore
{
	class Stopwatch final
	{
		using Valuesec = std::chrono::duration<double>;
		using Millisec = std::chrono::milliseconds;
		using Microsec = std::chrono::microseconds;
		using Nanosec = std::chrono::nanoseconds;

		std::chrono::steady_clock::duration elapsed()noexcept;

	public:
		Stopwatch()noexcept;

		double dt_float() noexcept;

		std::size_t dt_millisec() noexcept;

		std::size_t dt_microsec() noexcept;

		std::size_t dt_nanosec() noexcept;

		void reset()noexcept;

	private:
		std::chrono::steady_clock::time_point mWatch;
	};


	/*
	* DEPRICATED, FUNCTION IS NOT COMPLETE
	auto timeMyFunction = [](auto&& func, auto&&... args)->float {
		Stopwatch timer;

		std::forward<decltype(func)>(func)(
			std::forward<decltype(args)>(args)...
			);
		return timer.dt_float();
		};
		*/
}