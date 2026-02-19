#pragma once

#include <string>

namespace badCore
{
	struct BoolMessage
	{
		bool isGood;
		std::string message;

		BoolMessage(bool is_good, const std::string& msg)
			:isGood(is_good), message(msg)
		{
		}


		static BoolMessage success(std::string msg = {})noexcept
		{
			return { true, std::move(msg) };
		}

		static BoolMessage failure(std::string msg)noexcept
		{
			return { false, std::move(msg) };
		}

		explicit operator bool() const noexcept
		{
			return isGood;
		}
	};
}