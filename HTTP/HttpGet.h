#pragma once

#include <string>
#include "HttpSetopt.h"
#include "UCURL.h"
#include "HttpResponse.h"
#include "bString.h"

namespace badHTTP
{
	badCore::bString http_bind_get_template(CURL* curl, const std::string& cert, const std::string& url, HttpResponse& reciever)
	{
		CURLcode code;

		code = set_cacert(curl, cert);
		if (code != CURLE_OK) return setopt_failure(code);

		code = set_url(curl, url);
		if (code != CURLE_OK) return setopt_failure(code);

		code = set_write_function(curl);
		if (code != CURLE_OK) return setopt_failure(code);

		code = set_reciever(curl, reciever);
		if (code != CURLE_OK) return setopt_failure(code);

		code = set_max_redirects(curl, 2L);//magic 2, generally should be just 1 tho since i should know exactly where i am going
		if (code != CURLE_OK) return setopt_failure(code);

		code = set_max_transfer_time(curl, 5L);//meant to just be for jsons not heavy files
		if (code != CURLE_OK) return setopt_failure(code);

		code = set_max_connection_phase(curl, 5L);//if connects for 5 sec just abort, too slow
		if (code != CURLE_OK) return setopt_failure(code);

		code = set_ssl_verify(curl);
		if (code != CURLE_OK) return setopt_failure(code);

		return badCore::bString::success();
	}

	//does little to no checking. assumes set options are done up front
	badCore::bString execute_get(CURL* curl, HttpResponse& out)
	{
		if (!curl)
			return badCore::bString::failure("uninitalized curl");

		out.reset();

		CURLcode perform = curl_easy_perform(curl);

		if (perform != CURLE_OK)
			return badCore::bString::failure(std::string("curl_easy_perform failed: ") + curl_easy_strerror(perform));

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