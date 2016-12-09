#include <string>
#include <memory>
#include <functional>
#include <algorithm>

#include <cstdint>

#include "hs_crypt.h"
#include "hs_exception.h"

#define CBC 1

extern "C"
{
#include "aes.h"
#include "md5.h"
}

using crypt_unique_ptr = std::unique_ptr<uint8_t[]>;

void pkcs5_padding(std::vector<uint8_t> &buf, uint8_t block_size);
uint32_t pkcs5_size(const std::vector<uint8_t> &buf);
crypt_unique_ptr aes128iv();
crypt_unique_ptr aes128key(const std::string &password);

std::vector<uint8_t> hs_encrypt(const uint8_t *src, uint32_t len, const std::string &password)
{
	crypt_unique_ptr key = aes128key(password);
	crypt_unique_ptr iv = aes128iv();

	if (src == nullptr)
	{
		throw hs_exception("encrypt error.");
	}

	std::vector<uint8_t> buf(src, src + len);
	pkcs5_padding(buf, 16);
	std::vector<uint8_t> res(buf.size());
	AES128_CBC_encrypt_buffer(&res[0], &buf[0], buf.size(), key.get(), iv.get());

	return res;
}

std::vector<uint8_t> hs_decrypt(const uint8_t *src, uint32_t len, const std::string &password)
{
	crypt_unique_ptr key = aes128key(password);
	crypt_unique_ptr iv = aes128iv();

	if (src == nullptr || len == 0 || len % 16 != 0)
	{
		throw hs_exception("decrypt error.");
	}
	std::vector<uint8_t> res(len);
	AES128_CBC_decrypt_buffer(&res[0], const_cast<uint8_t *>(src), len, key.get(), iv.get());
	res.resize(pkcs5_size(res));

	return res;
}

std::vector<uint8_t> hs_encrypt(const std::vector<uint8_t> &src, const std::string &password)
{
	return hs_encrypt(&src[0], src.size(), password);
}

std::vector<uint8_t> hs_decrypt(const std::vector<uint8_t> &src, const std::string &password)
{
	return hs_decrypt(&src[0], src.size(), password);
}

void pkcs5_padding(std::vector<uint8_t> &buf, uint8_t block_size)
{
	uint32_t padded_length = 0;
	uint32_t current_length = buf.size();
	if (current_length % block_size == 0)
	{
		padded_length = current_length + block_size;
	}
	else
	{
		padded_length = (current_length / block_size + 1) * block_size;
	}

	buf.resize(padded_length, padded_length - current_length);
}

uint32_t pkcs5_size(const std::vector<uint8_t> &buf)
{
	auto length = buf.size();
	return length - buf[length - 1];
}

crypt_unique_ptr aes128iv()
{
	crypt_unique_ptr p(new uint8_t[16]);
	std::fill_n(p.get(), 16, 0);
	return p;
}

crypt_unique_ptr aes128key(const std::string &password)
{
	MD5_CTX ctx;
	MD5_Init(&ctx);
	MD5_Update(&ctx, password.c_str(), password.length());
	crypt_unique_ptr key(new uint8_t[16]);
	MD5_Final(key.get(), &ctx);
	return key;
}

/*
#include <cstdio>
#include <iostream>
int main(void)
{
	uint8_t *data = (uint8_t *)"abcdefgha;sldkfjasdl;fhk23907vzy9olxhfa;sdkljh348oaysdhjkgah;sdfjka;sdf";
	auto e = hs_encrypt(data, 30, "pass");
	for (auto c : e)
	{
		printf("%02x, ", c);
	}
	printf("\n");

	auto d = hs_decrypt(e, "pass");
	for (auto c : d)
	{
		printf("%02x, ", c);
	}
	printf("\n");
	std::cout << std::string((char *)(&d[0]), d.size()) << std::endl;

    return 0;
}
*/
