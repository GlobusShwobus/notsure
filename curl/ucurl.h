#pragma once
#include <memory>
#include <curl/curl.h>

namespace badCurl
{
	struct CURL_DELETER {
		void operator()(CURL* c)const {
			if (c)
				curl_easy_cleanup(c);
		}
	};

	using UCURL = std::unique_ptr<CURL, CURL_DELETER>;

	struct CURLM_DELETER {
		void operator()(CURLM* m) {
			if (m)
				curl_multi_cleanup(m);
		}
	};

	using UCURLM = std::unique_ptr<CURLM, CURLM_DELETER>;
}