#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

typedef struct hs_settings_
{
	const char *remote_url;
	unsigned int interval;
} hs_settings;

int signal == 0;

void daemonize();
int hs_setting_init(hs_settings *setting, int argc, char *argv[]);
int hs(hs_settings *setting);

uint8_t *fetch_hs_data(const char *url);
void free_hs_data(uint8_t *data);

int main(int argc, char *argv[])
{
	daemonize();

	struct hs_settings setting;
	int r = hs_setting_init(&setting, argc, argv);
	if (r != 0) return r;

	return hs(setting);
}

void daemonize()
{
	return;
}

int hs_setting_init(hs_settings *setting, int argc, char *argv[])
{
	return 0;
}

int hs(struct hs_settings *setting)
{
	while (signal == 0)
	{
		uint8_t *data = fetch_hs_data(setting->remote_url);

		if (hs_check_data(data))
		{
			hs_bencode b;
			hs_bencode_init(&b, data);

			hs_parse_setting(setting, b);
			hs_parse_command(setting, b);
		}

		free_hs_data(data);

		sleep(setting->interval);
	}
	return 0;
}

uint8_t *fetch_hs_data(const char *url)
{
	struct http_response *hrep = http_get(url, NULL, getenv("http_proxy"));
	printf("%d: %s\n%s\n", hrep->status_code_int, hrep->response_headers, hrep->body);
	http_response_free(hrep);
}

void free_hs_data(uint8_t *data)
{
	free(data);
}

// ld10:remote_url22:http://127.0.0.1:8080/8:intervali10eel2:ls3:who6:whoamiee
