#include <cstring>
#include <string>

#include "hs_compress.h"
#include "hs_exception.h"

extern "C"
{
#include "tinf.h"
#include "defl_static.h"
}

static void uzlib_buf_free(unsigned char *buf)
{
	free(buf);
}

constexpr size_t gzip_meta_size()
{
	return sizeof(uint8_t) * 4 + sizeof(uint32_t) + sizeof(uint8_t) * 2 + sizeof(uint32_t) * 2;
}

std::vector<uint8_t> hs_gzip(const uint8_t *src, size_t len)
{
    struct Outbuf out = {0};
    zlib_start_block(&out);
    uzlib_compress(&out, src, len);
    zlib_finish_block(&out);

	std::vector<uint8_t> ret(out.outlen + gzip_meta_size());
	ret[0] = 0x1f;
	ret[1] = 0x8b;
	ret[2] = 0x08;
	ret[3] = 0x00; // FLG
    int mtime = 0;
	memcpy(&ret[4], &mtime, sizeof(mtime));
	ret[8] = 0x04; // XFL
	ret[9] = 0x03; // OS

	memcpy(&ret[10], out.outbuf, out.outlen);

    uint32_t crc = ~uzlib_crc32(src, len, ~0);
	memcpy(&ret[10 + out.outlen], &crc, sizeof(crc));
	uint32_t dlen = len;
	memcpy(&ret[10 + out.outlen + 4], &dlen, sizeof(dlen));

	uzlib_buf_free(out.outbuf);

	return ret;
}

std::vector<uint8_t> hs_gunzip(const uint8_t *src, size_t len)
{
    uzlib_init();

    uint32_t dlen;
	memcpy(&dlen, &src[len-4], sizeof(dlen));

	std::vector<uint8_t> ret(dlen);

    TINF_DATA d;
    d.source = src;

    int res = uzlib_gzip_parse_header(&d);
    if (res != TINF_OK)
	{
        throw hs_exception("Error parsing gzip header");
    }
    uzlib_uncompress_init(&d, NULL, 0);
    d.dest = &ret[0];
    d.destSize = 1;

    do {
        res = uzlib_uncompress_chksum(&d);
    } while (res == TINF_OK);

    if (res != TINF_DONE)
	{
        throw hs_exception("Error during gzip decompression");
    }

	return ret;
}

std::vector<uint8_t> hs_gzip(const std::vector<uint8_t> &src)
{
	return hs_gzip(&src[0], src.size());
}

std::vector<uint8_t> hs_gunzip(const std::vector<uint8_t> &src)
{
	return hs_gunzip(&src[0], src.size());
}

/*
#include <iostream>
int main()
{
	auto s = "abcdefghijklmnopqrstuvwxyzaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n";
	std::vector<unsigned char> a(s, s + 26);
	auto b = hs_gzip(a);
	auto c = hs_gunzip(b);
	for (auto d : c)
	{
		std::cout << d;
	}
	return 0;
}
*/
