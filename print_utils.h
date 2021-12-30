#pragma once

#include <immintrin.h>

void p256_chars(char const *s, __m256i in);
void p256_hex_u8(char const *s, __m256i in);
void p128_hex_u8(char const *s, __m128i in);
void p256_hex_u16(char const *s, __m256i in);
void p256_hex_u32(char const *s, __m256i in);
void p256_hex_u64(char const *s, __m256i in);
