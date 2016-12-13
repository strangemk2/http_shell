#include <memory>
#include <functional>
#include <sstream>

#include "hs_http.h"
#include "hs_exception.h"

extern "C"
{
#include "http-client-c.h"
}

using http_response_ptr = std::unique_ptr<http_response, std::function<void(http_response *)>>;

http_response_ptr make_http_response_ptr(http_response *response)
{
	return http_response_ptr(response, [](http_response *p){ http_response_free(p); });
}

static void split(const std::string &s, char delim, std::vector<std::string> &elems)
{
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim))
	{
        elems.push_back(item);
    }
}

// static std::vector<std::string> split(const std::string &s, char delim)
// {
//     std::vector<std::string> elems;
//     split(s, delim, elems);
//     return elems;
// }

using http_request_function = std::function<http_response *(const char *)>;

hs_http_response hs_http_request_process(const std::string &url, http_request_function f)
{
	auto hrep = make_http_response_ptr(f(url.c_str()));
	if (hrep.get() == nullptr)
	{
		throw hs_exception("http request error.");
	}

	hs_http_response ret;
	ret.status_code = hrep->status_code_int;
	split(hrep->response_headers, '\n', ret.headers);
	ret.body = hrep->body;
	return ret;
}

hs_http_response hs_http_get(const std::string &url, const std::string custom_header, const std::string proxy)
{
	return hs_http_request_process(url,
			std::bind(http_get,
				      std::placeholders::_1,
					  custom_header.empty()?nullptr:custom_header.c_str(),
					  proxy.empty()?nullptr:proxy.c_str()));
}

hs_http_response hs_http_post(const std::string &url, const std::string &post_data, const std::string custom_header, const std::string proxy)
{
	return hs_http_request_process(url,
			std::bind(http_post,
				      std::placeholders::_1,
					  custom_header.empty()?nullptr:custom_header.c_str(),
					  post_data.c_str(),
					  proxy.empty()?nullptr:proxy.c_str()));
}

/*
#include <iostream>
int main()
{
	auto a = hs_http_post("http://www.nba.com", "", "", "http://web-proxy.jpn.hp.com:8080/");
	std::cout << a.status_code << std::endl;
	for (auto l : a.headers)
	{
		std::cout << "h: " << l << std::endl;
	}
	std::cout << a.body;

	return 0;
}
*/
