#pragma once

// Simple wrapper for base64

#include <vector>

std::vector<unsigned char> hs_base64_encode(const unsigned char *src, size_t len);
std::vector<unsigned char> hs_base64_decode(const unsigned char *src, size_t len);

std::vector<unsigned char> hs_base64_encode(const std::vector<unsigned char> &src);
std::vector<unsigned char> hs_base64_decode(const std::vector<unsigned char> &src);
