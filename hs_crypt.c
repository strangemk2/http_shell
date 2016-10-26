#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define CBC 1

#include "aes.h"
#include "md5.h"

uint32_t pkcs5_padding(uint8_t **out, uint8_t *buf, uint32_t length, uint8_t block_size);
uint32_t pkcs5_size(uint8_t *buf, uint32_t length);
uint8_t *aes128iv();
uint8_t *aes128key(const char *password);

uint32_t encrypt(uint8_t **out, uint8_t *buf, uint32_t length, const char* password)
{
	uint8_t *key = aes128key(password);
	uint8_t *iv = aes128iv();

	if (buf == NULL)
	{
		return 0;
	}

	uint8_t *in = NULL;
	uint32_t encrypted_len = pkcs5_padding(&in, buf, length, 16);
	*out = (uint8_t *)malloc(encrypted_len);
	if (in == NULL || *out == NULL)
	{
		return 0;
	}

	AES128_CBC_encrypt_buffer(*out, in, encrypted_len, key, iv);

	free(key);
	free(iv);
	free(in);

	return encrypted_len;
}

uint32_t decrypt(uint8_t **out, uint8_t *buf, uint32_t length, const char* password)
{
	uint8_t *key = aes128key(password);
	uint8_t *iv = aes128iv();

	if (buf == NULL || length == 0 || length % 16 != 0)
	{
		return 0;
	}
	*out = (uint8_t *)malloc(length);
	if (*out == NULL)
	{
		return 0;
	}

	AES128_CBC_decrypt_buffer(*out, buf, length, key, iv);

	free(key);
	free(iv);

	return pkcs5_size(*out, length);
}

uint32_t pkcs5_padding(uint8_t **out, uint8_t *buf, uint32_t length, uint8_t block_size)
{
	uint32_t padded_length = 0;
	if (length % block_size == 0)
	{
		padded_length = length + block_size;
	}
	else
	{
		padded_length = (length / 16 + 1) * block_size;
	}

	*out = (uint8_t *)malloc(padded_length);
	if (*out == NULL)
	{
		return 0;
	}
	memcpy(*out, buf, length);
	memset(*out + length, (uint8_t)(padded_length - length), (padded_length - length));

	return padded_length;
}

uint32_t pkcs5_size(uint8_t *buf, uint32_t length)
{
	return length - buf[length - 1];
}

uint8_t *aes128iv()
{
	uint8_t *iv = (uint8_t *)malloc(16);
	if (iv != NULL) memset(iv, 0, 16);
	return iv;
}

uint8_t *aes128key(const char *password)
{
	MD5_CTX ctx;
	MD5_Init(&ctx);
	MD5_Update(&ctx, password, strlen(password));
	uint8_t *key = (uint8_t *)malloc(16);
	if (key != NULL) MD5_Final(key, &ctx);
	return key;
}

/*
int main(void)
{
	uint8_t *data = (uint8_t *)"abcdefgha;sldkfjasdl;fhk23907vzy9olxhfa;sdkljh348oaysdhjkgah;sdfjka;sdf";
	uint8_t *p = NULL;
	uint32_t l = encrypt(&p, data, 5, "pass");
	for (int i = 0; i < l; i++)
	{
		printf("%02x, ", p[i]);
	}
	printf("\n");

	uint8_t *q = NULL;
	uint32_t m = decrypt(&q, p, l, "pass");
	for (int i = 0; i < m; i++)
	{
		printf("%02x, ", q[i]);
	}
	printf("\n");
	for (int i = 0; i < m; i++)
	{
		printf("%02x, ", data[i]);
	}
	printf("\n");

	free(p);
	free(q);

    return 0;
}
*/
