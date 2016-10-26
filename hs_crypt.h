#pragma once

uint32_t encrypt(uint8_t **out, uint8_t *buf, uint32_t length, const char* password);
uint32_t decrypt(uint8_t **out, uint8_t *buf, uint32_t length, const char* password);
