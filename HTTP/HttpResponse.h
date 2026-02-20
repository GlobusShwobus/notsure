#pragma once
#include <string>

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
	};

	static std::size_t write_callback(char* content, std::size_t size, std::size_t nmemb, void* clientp)
	{
		auto* resp = static_cast<HttpResponse*>(clientp);
		resp->body.append(content, size * nmemb);

		return size * nmemb;
	}
}