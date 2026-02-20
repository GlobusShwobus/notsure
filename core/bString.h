#pragma once

#include <string>

//TODO:: constexpr

namespace badCore
{
	struct bString
	{
		std::string message;

		static bString success()noexcept
		{
			return {};
		}

		static bString failure(std::string msg)noexcept
		{
			if (msg.empty())
				msg = "undocumented error";
			
			return { std::move(msg) };
		}

		bool succeeded()const noexcept
		{
			return message.empty();
		}

		bool failed()const noexcept
		{
			return !message.empty();
		}

		explicit operator bool() const noexcept
		{
			return succeeded();
		}
	};
}