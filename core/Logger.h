#pragma once

#include <string>
#include <mutex>
#include "Sequence.h"

namespace badCore
{
	class Logger final
	{
	public:
		Logger(const Logger&) = delete;
		Logger& operator=(const Logger&) = delete;
		Logger(Logger&&) = delete;
		Logger& operator=(Logger&&) = delete;

		static Logger& instance() noexcept
		{
			static Logger instance;
			return instance;
		}

		template <typename... Args>
		void add_log(Args&&... args)
			requires std::constructible_from<std::string, Args...>
		{
			std::lock_guard<std::mutex> lock(mMutex);
			mLogs.emplace_back(std::forward<Args>(args)...);
		}

		template <typename... Args>
		void add_error(Args&&... args)
			requires std::constructible_from<std::string, Args...>
		{
			std::lock_guard<std::mutex> lock(mMutex);
			mErrors.emplace_back(std::forward<Args>(args)...);
		}

		inline const auto& get_logs()const noexcept
		{
			return mLogs;
		}

		inline const auto& get_errors()const noexcept
		{
			return mErrors;
		}

		// returns last log or empty if none exist
		inline std::string last_log()const noexcept
		{
			return mLogs.isEmpty() ? "" : mLogs.back();
		}

		// returns last log or empty if none exist
		inline std::string last_error()const noexcept
		{
			return mErrors.isEmpty() ? "" : mErrors.back();
		}

	private:
		Logger() = default;
		~Logger() = default;

		mutable std::mutex mMutex;
		Sequence<std::string> mLogs;
		Sequence<std::string> mErrors;
	};
}