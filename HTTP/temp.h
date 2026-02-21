#pragma once

#include <string>
#include "Logger.h"
#include <span>
#include "Sequence.h"
#include <curl/curl.h>
#include "BoolMessage.h"
#include "WRITE_HANDLE.h"
#include "ucurl.h"

namespace badCurl
{
	struct CurlRequest {
		UCURL curl = UCURL(curl_easy_init());
		WRITE_HANDLE handle;

		CURLcode setup(const std::string& url, std::string_view cert)
		{
			CURL* c = curl.get();
			CURLcode rc;

			//basic setup
			rc = curl_easy_setopt(c, CURLOPT_URL, url.data());
			if (rc != CURLE_OK) return rc;

			rc = curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, read_callback);
			if (rc != CURLE_OK) return rc;

			rc = curl_easy_setopt(c, CURLOPT_WRITEDATA, &handle);
			if (rc != CURLE_OK) return rc;

			// Store pointer to THIS object (stable storage)
			rc = curl_easy_setopt(c, CURLOPT_PRIVATE, this);
			if (rc != CURLE_OK) return rc;

			//essential options
			curl_easy_setopt(c, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(c, CURLOPT_MAXREDIRS, 5L);
			curl_easy_setopt(c, CURLOPT_TIMEOUT, 30L);
			curl_easy_setopt(c, CURLOPT_CONNECTTIMEOUT, 10L);
			curl_easy_setopt(c, CURLOPT_SSL_VERIFYPEER, 1L);
			curl_easy_setopt(c, CURLOPT_SSL_VERIFYHOST, 2L);

			if (!cert.empty()) {
				rc = curl_easy_setopt(c, CURLOPT_CAINFO, cert.data());
				if (rc != CURLE_OK) return rc;
			}

			return CURLE_OK;
		}
	};

	badCore::Sequence<WRITE_HANDLE> multi_request_data(std::span<const std::string> urls, std::string_view certificate)
	{
		badCore::Logger& logger = badCore::Logger::instance();
		UCURLM ucurlm(curl_multi_init());
		CURLM* curlm = ucurlm.get();

		// if failed init
		if (!curlm) {
			logger.add_error("curl_multi_init failed");
			return {};
		}
		//phase 1: set up handles, and curl objects and push them to multihandle

		badCore::Sequence<CurlRequest> requests;
		requests.set_capacity(urls.size());

		for (auto& url : urls) {
			requests.emplace_back();
			auto& req = requests.back();

			CURLcode result = req.setup(url, certificate);

			if (result != CURLE_OK) {
				logger.add_error(std::string("FAIL setup: ") + curl_easy_strerror(result) + " [" + std::string(url) + "]");
				requests.pop_back();
				continue;
			}
			curl_multi_add_handle(curlm, req.curl.get());
		}

		//phase 2: preform multi curl
		int still_running = 1;
		while (still_running) {
			CURLMcode mrc = curl_multi_perform(curlm, &still_running);
			if (mrc != CURLM_OK && mrc != CURLM_CALL_MULTI_PERFORM) {
				logger.add_error(std::string("curl_multi_perform failed: ") + curl_multi_strerror(mrc));
				break;//if the CURLM itself broke not individual call, at that point nothing left to do
			}

			if (still_running) {
				//wait for activity 1000 ms
				mrc = curl_multi_poll(curlm, nullptr, 0, 1000, nullptr);

				if (mrc != CURLM_OK) {
					logger.add_error(std::string("curl_multi_poll failed: ") + curl_multi_strerror(mrc));
				}
			}
		}

		//phase 3: collect results

		CURLMsg* msg = nullptr;
		int msgs_left = 0;
		badCore::Sequence<WRITE_HANDLE> results;

		while ((msg = curl_multi_info_read(curlm, &msgs_left))) {

			if (msg->msg == CURLMSG_DONE) {
				CurlRequest* req = nullptr;

				curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &req);
				if (req) {
					//http code
					curl_easy_getinfo(msg->easy_handle, CURLINFO_RESPONSE_CODE, &req->handle.httpcode);

					//content type
					char* content_type = nullptr;
					curl_easy_getinfo(msg->easy_handle, CURLINFO_CONTENT_TYPE, &content_type);

					if (content_type) {
						req->handle.contentType = content_type;
					}

					req->handle.is_good = (msg->data.result == CURLE_OK &&
						req->handle.httpcode >= 200 &&
						req->handle.httpcode < 300);

					results.push_back(std::move(req->handle));
				}
			}
			curl_multi_remove_handle(curlm, msg->easy_handle);
		}

		return results;
	}

	/*
	NOTE:: CURL GLOBAL INIT DONE ONCE PER PROGRAM, NOT FUNCTION CALL

		CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
		if (result != CURLE_OK) {
			logger.add_error("Global init fail"+ std::string(curl_easy_strerror(result)));
			return handles;
		}

		curl_global_cleanup();
	*/
}