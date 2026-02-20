#pragma once

#include <string>
#include "UCURL.h"
//SHIT IDEA DELETE THIS NOW
namespace badCurl
{
	class StickyCurl
	{
	public:
		StickyCurl() noexcept
		:c(curl_easy_init())
		{
		}

		CURL* get() noexcept 
		{ 
			return c.get();
		}
		const CURL* get() const noexcept 
		{ 
			return c.get();
		}

		bool valid()const noexcept
		{
			return static_cast<bool>(c);
		}

		void reset() noexcept
		{
			if (c) {
				curl_easy_reset(c.get());
			}
		}

		CURLcode perform()
		{
			if (!c)
				return CURLE_FAILED_INIT;

			return curl_easy_perform(c.get());
		}

		template <typename T>
		CURLcode set_option(CURLoption opt, T val)
		{
			if (!c)
				return CURLE_FAILED_INIT;

			return curl_easy_setopt(c.get(), opt, val);
		}

		StickyCurl duplicate()const noexcept
		{
			if (!c)
				return StickyCurl(nullptr);

			return StickyCurl(curl_easy_duphandle(get()));
		}

	private:
		
		explicit StickyCurl(CURL* raw)noexcept
			:c(raw)
		{
		}

	private:
		UCURL c;
	};
}