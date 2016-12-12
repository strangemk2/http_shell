#pragma once

// Simple wrapper for uzlib

#include <vector>

std::vector<uint8_t> hs_gz(const uint8_t *src, size_t len);
std::vector<uint8_t> hs_gunz(const uint8_t *src, size_t len);

std::vector<uint8_t> hs_gz(const std::vector<uint8_t> &src);
std::vector<uint8_t> hs_gunz(const std::vector<uint8_t> &src);
