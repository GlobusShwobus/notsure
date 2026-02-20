#pragma once

#include <string>
#include "HttpSetopt.h"
#include "UCURL.h"
#include "HttpResponse.h"
#include "bString.h"

namespace badHTTP
{
	CURLcode test_connection(CURL* curl, const std::string& url, const std::string& pem)
	{
		CURLcode code;
		if (curl)
			return CURLE_FAILED_INIT;

		if (code = set_url(curl, url); code != CURLE_OK)
			return code;

		if (code = set_cacert(curl, pem); code != CURLE_OK)
			return code;

		if (code = set_only_connect(curl); code != CURLE_OK)
			return code;


		return curl_easy_perform(curl);
	}

	//assumes all set options are done up front
	badCore::bString http_get(CURL* curl, HttpResponse& out)
	{
		if (!curl)
			return badCore::bString::failure("uninitalized curl");

		CURLcode perform = curl_easy_perform(curl);

		//TODO: check that out doesn't have any weird partial data. clean it up or something
		if (perform != CURLE_OK) {
			return badCore::bString::failure(std::string("curl_easy_perform failed: ") + curl_easy_strerror(perform));
		}

		out.transport_ok = true;//if curle_ok

		//http code
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &out.httpcode);
		
		//content type if that field is set by content
		char* ct = nullptr;
		curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
		if (ct) out.contentType = ct;

		out.http_ok = out.httpcode >= 200 && out.httpcode < 300;

		return badCore::bString::success();
	}

}