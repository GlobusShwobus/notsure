#pragma once

#include <string>
#include "HttpResponse.h"
#include "bString.h"
#include "UCURL.h"

namespace badHTTP
{
	badCore::bString setopt_failure(CURLcode code)noexcept
	{
		return badCore::bString::failure(curl_easy_strerror(code));
	}

	CURLcode set_cacert(CURL* curl, const std::string& path) noexcept
	{
		return curl_easy_setopt(curl, CURLOPT_CAINFO, path.data());
	}

	CURLcode set_url(CURL* curl, const std::string& url) noexcept
	{
		return curl_easy_setopt(curl, CURLOPT_URL, url.data());
	}

	CURLcode set_write_function(CURL* curl) noexcept
	{
		return curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
	}

	CURLcode set_ssl_verify(CURL* curl) noexcept
	{
		CURLcode code = CURLE_OK;

		code = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
		if (code!=CURLE_OK)
			return code;

		code = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
		if (code != CURLE_OK)
			return code;

		return code;
	}

	CURLcode set_max_connection_phase(CURL* curl, long time) noexcept
	{
		return curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, time);
	}

	CURLcode set_max_transfer_time(CURL* curl, long time) noexcept
	{
		return curl_easy_setopt(curl, CURLOPT_TIMEOUT, time);
	}

	CURLcode set_max_redirects(CURL* curl, long count) noexcept
	{
		CURLcode code = CURLE_OK;

		code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		if (code != CURLE_OK)
			return code;

		code = curl_easy_setopt(curl, CURLOPT_MAXREDIRS, count);
		if (code != CURLE_OK)
			return code;

		return code;
	}

	CURLcode set_reciever(CURL* curl, HttpResponse& reciever) noexcept
	{
		return curl_easy_setopt(curl, CURLOPT_WRITEDATA, &reciever);
	}

	CURLcode set_only_connect(CURL* curl) noexcept
	{
		return curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 1L);
	}
}