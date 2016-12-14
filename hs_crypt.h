#pragma once

// Simple wrapper for tiny-AES128-C

#include <string>
#include <vector>

std::vector<uint8_t> hs_encrypt(const uint8_t *src, uint32_t len, const std::string &password);
std::vector<uint8_t> hs_decrypt(const uint8_t *src, uint32_t len, const std::string &password);

std::vector<uint8_t> hs_encrypt(const std::vector<uint8_t> &src, const std::string &password);
std::vector<uint8_t> hs_decrypt(const std::vector<uint8_t> &src, const std::string &password);
