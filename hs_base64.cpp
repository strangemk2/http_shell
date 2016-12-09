#include <string>
#include <memory>
#include <functional>

#include "hs_base64.h"
#include "hs_exception.h"

extern "C"
{
#include "base64.h"
}

using base64_unique_ptr = std::unique_ptr<unsigned char, std::function<void(unsigned char *)>>;

template <typename F>
std::vector<unsigned char> hs_base64_impl(const unsigned char *src, size_t len, F f)
{
	size_t l = 0;
	base64_unique_ptr s(f(src, len, &l), [](unsigned char *buf){ if (buf) free(buf); });
	if (s.get() == nullptr)
	{
		throw hs_exception("base64 encode/decode error.");
	}
	std::vector<unsigned char> ret(s.get(), s.get() + l);
	return ret;
}

std::vector<unsigned char> hs_base64_encode(const unsigned char *src, size_t len)
{
	return hs_base64_impl(src, len, base64_encode);
}

std::vector<unsigned char> hs_base64_decode(const unsigned char *src, size_t len)
{
	return hs_base64_impl(src, len, base64_decode);
}

std::vector<unsigned char> hs_base64_encode(const std::vector<unsigned char> &src)
{
	return hs_base64_encode(&src[0], src.size());
}

std::vector<unsigned char> hs_base64_decode(const std::vector<unsigned char> &src)
{
	return hs_base64_decode(&src[0], src.size());
}

/*
#include <iostream>
int main()
{
	auto s = "abcde";
	std::vector<unsigned char> a(s, s + 5);
	auto b = hs_base64_encode(a);
	auto c = hs_base64_decode(b);
	std::cout << std::string(b.begin(), b.end()) << std::endl;
	std::cout << std::string(c.begin(), c.end()) << std::endl;
	return 0;
}
*/
