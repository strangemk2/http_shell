#pragma once

// Simple wrapper for http-client-c

#include <string>
#include <vector>

class hs_http_response
{
	public:
		int status_code = 0;
		std::vector<std::string> headers;
		std::string body;
};

hs_http_response hs_http_get(const std::string &url, const std::string custom_header = "", const std::string proxy = "");
hs_http_response hs_http_post(const std::string &url, const std::string &post_data, const std::string custom_header = "", const std::string proxy = "");
