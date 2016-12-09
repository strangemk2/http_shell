#pragma once

#include <stdint.h>

uint32_t compress(uint8_t **out, uint8_t *buf, uint32_t length);
uint32_t decompress(uint8_t **out, uint8_t *buf, uint32_t length);
