#pragma once

// Simple wrapper for uzlib

#include <vector>

std::vector<uint8_t> hs_gzip(const uint8_t *src, size_t len);
std::vector<uint8_t> hs_gunzip(const uint8_t *src, size_t len);

std::vector<uint8_t> hs_gzip(const std::vector<uint8_t> &src);
std::vector<uint8_t> hs_gunzip(const std::vector<uint8_t> &src);
