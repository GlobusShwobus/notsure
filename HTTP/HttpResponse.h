#pragma once
#include <string>
#include <curl/curl.h>

namespace badHTTP
{
	struct HttpResponse 
	{
		std::string body;
		std::string contentType;
		long httpcode = 0;
		bool transport_ok = false;
		bool http_ok = false;

		explicit operator bool() const noexcept
		{
			return transport_ok && http_ok;
		}

		void reset() noexcept
		{
			body.clear();
			contentType.clear();
			httpcode = 0;
			transport_ok = false;
			httpcode = false;
		}
	};

	static std::size_t write_callback(char* content, std::size_t size, std::size_t nmemb, void* clientp)
	{
		auto* resp = static_cast<HttpResponse*>(clientp);
		resp->body.append(content, size * nmemb);

		return size * nmemb;
	}

	CURLcode test_connection(CURL* curl, const std::string& url, const std::string& pem)
	{
		CURLcode code;
		if (curl)
			return CURLE_FAILED_INIT;

		if (code = curl_easy_setopt(curl, CURLOPT_URL, url.data()); code != CURLE_OK)
			return code;

		if (code = curl_easy_setopt(curl, CURLOPT_CAINFO, pem.data()); code != CURLE_OK)
			return code;

		if (code = curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 1L); code != CURLE_OK)
			return code;


		return curl_easy_perform(curl);
	}
}