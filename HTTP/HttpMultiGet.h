#pragma once

#include "HttpSetopt.h"
#include "HttpResponse.h"
#include "UCURL.h"
#include "Sequence.h"
#include "bString.h"
#include <algorithm>

namespace badHTTP
{
	struct steady_handle
	{
		UCURL curl;
		HttpResponse response;

		steady_handle() = default;
		steady_handle(const steady_handle&) = delete;
		steady_handle& operator=(const steady_handle&) = delete;
		steady_handle(steady_handle&&) noexcept = default;
		steady_handle& operator=(steady_handle&&) noexcept = default;
	};

	class MultiGET final
	{
	public:
		MultiGET() :curlm(curl_multi_init()) {}

		CURLM* get()noexcept {
			return curlm.get();
		}

		badCore::bString add_handle(const std::string& url, CURL* copy_from_base = nullptr)
		{
			if(!curlm.get())
				return badCore::bString::failure("uninitalized curlm");

			handles.emplace_back();

			steady_handle& handle = handles.back();
			handle.curl.reset(
				(copy_from_base == nullptr) ? curl_easy_init() : curl_easy_duphandle(copy_from_base)
			);
			
			CURL* curl = handle.curl.get();
			if (curl == nullptr)
				return badCore::bString::failure("failed to initalize curl");

			auto bind_handle = set_url_and_receiver(curl, url, handle.response);

			if (bind_handle != CURLE_OK)
				return badCore::bString::failure(curl_easy_strerror(bind_handle));

			auto bind_multi = curl_multi_add_handle(curlm.get(), curl);

			if (bind_multi != CURLM_OK) {
				handles.pop_back();
				return badCore::bString::failure(curl_multi_strerror(bind_multi));
			}
		
			return badCore::bString::success();
		}

		badCore::bString add_handles(const badCore::Sequence<std::string>& urls, CURL* copy_from_base = nullptr)
		{
			if (curlm.get() == nullptr)
				return badCore::bString::failure("uninitalized curlm");

			if (urls.isEmpty())
				return badCore::bString::failure("no urls");


			for (const auto& url : urls) {

				auto result = add_handle(url, copy_from_base);

				//TODO:: decide to fail fully or continue...
				if (result.failed())
					return result;

			}

			return badCore::bString::success();
		}

		void clear() noexcept {
			if (curlm) {
				CURLM* pcurlm = curlm.get();
				for (auto& h : handles) {
					if (CURL* curl = h.curl.get())
						curl_multi_remove_handle(pcurlm, curl);
				}
				curl_multi_cleanup(pcurlm);
			}
			handles.clear();
		}

		//explicit i guess
		MultiGET(const MultiGET&) = delete;
		MultiGET& operator=(const MultiGET&) = delete;

		~MultiGET() {
			clear();
		}

	private:
		UCURLM curlm;
		badCore::Sequence<steady_handle> handles;
	};

	//peepee poopoo
	badCore::bString execute_multi_get(
		CURL* base, 
		const badCore::Sequence<std::string>& urls,
		badCore::Sequence<HttpResponse>& responses)
	{
		if (!base)
			return badCore::bString::failure("Null base CURL handle");

		if (urls.isEmpty())
			return badCore::bString::success();

		// Prepare responses vector
		responses.clear();
		responses.resize(urls.size());
	}
}