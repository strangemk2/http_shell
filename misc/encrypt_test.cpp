// g++ -std=c++11 -Wall encrypt_test.cpp hs_crypt.o tiny-AES128-C/md5.o tiny-AES128-C/aes.o hs_base64.o base64.o hs_compress.o -Luzlib/lib -ltinf

#include <iostream>
#include <string>

#include "hs_crypt.h"
#include "hs_compress.h"
#include "hs_base64.h"
#include "hs_exception.h"

int main()
{
	auto deflated = hs_gunzip(hs_decrypt(
				hs_base64_decode((const unsigned char *)"1Golj+yoVySpQSHXjBw7NjdBfw5u6pJAP8PYp0YcJjo=", 44),
				"random key"));
	std::cout << std::string(deflated.begin(), deflated.end()) << std::endl;

	const char *s = "abcdde";
	auto post_data = hs_base64_encode(
			hs_encrypt(
				hs_gzip((const unsigned char *)s, 6),
				"random key"));
	std::cout << std::string(post_data.begin(), post_data.end()) << std::endl;
}
