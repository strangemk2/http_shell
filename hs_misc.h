#pragma once

#include <string>
#include <vector>

std::string url_encode(const std::string &data);

std::string hs_exec(const std::string &cmd);

void split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);

constexpr const char* ws = " \t\n\r\f\v";
std::string& rtrim(std::string& s, const char* t = ws);
std::string& ltrim(std::string& s, const char* t = ws);
std::string& trim(std::string& s, const char* t = ws);
