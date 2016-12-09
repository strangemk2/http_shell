#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "base64.h"
#include "hs_crypt.h"

typedef struct hs_settings_
{
	char *remote_url;
	char *password;
	unsigned int interval;
} hs_settings;

int signal = 0;

void daemonize();
int hs_setting_init(hs_settings *setting, int argc, char *argv[]);
int hs(hs_settings *setting);
int hs_impl(hs_settings *setting);

uint8_t *fetch_hs_data(const char *url);
void free_hs_data(uint8_t *data);

uint8_t *hs_encrypt(uint8_t *src, size_t len, size_t *out_len, const char *password);
uint8_t *hs_decrypt(uint8_t *src, size_t len, size_t *out_len, const char *password);
bool hs_check_data(uint8_t *data);
bool hs_parse_setting(hs_settings *settings, hs_bencode *bencode);
bool hs_parse_command(hs_settings *settings, hs_bencode *bencode);

int main(int argc, char *argv[])
{
	daemonize();

	hs_settings setting;
	int r = hs_setting_init(&setting, argc, argv);
	if (r != 0) return r;

	return hs(&setting);
}

void daemonize()
{
	return;
}

int hs_setting_init(hs_settings *setting, int argc, char *argv[])
{
	return 0;
}

int hs(hs_settings *setting)
{
	while (signal == 0)
	{
		int ret = hs_impl(setting);
		if (ret != 0)
		{
			// logger
		}
		sleep(setting->interval);
	}
	return 0;
}

int hs_impl(hs_settings *setting)
{
	uint8_t *hs_data = fetch_hs_data(setting->remote_url);
	if (hs_data == NULL) return 1;

	size_t base64_decoded_len;
	uint8_t *base64_decoded = base64_decode(hs_data, strlen((char *)hs_data), &base64_decoded_len);
	free_hs_data(hs_data);
	if (base64_decoded == NULL) return 2;

	uint8_t *decrypted = hs_decrypt(base64_decoded, base64_decoded_len);
	free(base64_decoded);
	if (decrypted == NULL) return 3;

	if (hs_check_data(decrypted))
	{
		hs_parse_setting(setting, b);
		hs_parse_command(setting, b);
	}

	free(decrypted);
}

uint8_t *fetch_hs_data(const char *url)
{
	char *ret = NULL;
	struct http_response *hrep = http_get(url, NULL, getenv("http_proxy"));
	if (hrep && 200 <= hrep->status_code_int && hrep->status_code_int < 300)
	{
		ret =  strdup(hrep->body);
	}
	http_response_free(hrep);

	return ret;
}

void free_hs_data(uint8_t *data)
{
	free(data);
}

uint8_t *hs_encrypt(uint8_t *src, size_t len, size_t *out_len, const char *password)
{
	uint8_t *ret = NULL;
	*out_len = encrypt(&ret, src, len, password);
	return ret;
}

uint8_t *hs_decrypt(uint8_t *src, size_t len, size_t *out_len, const char *password)
{
	uint8_t *ret = NULL;
	*out_len = decrypt(&ret, src, len, password);
	return ret;
}

bool hs_check_data(uint8_t *data)
{
}

bool hs_parse_setting(hs_settings *settings, hs_bencode *bencode)
{
}

bool hs_parse_command(hs_settings *settings, hs_bencode *bencode)
{
}
