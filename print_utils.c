#include "print_utils.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

void p256_chars(char const *s, __m256i in) {
  uint8_t v[32];
  memcpy(v, &in, sizeof(in));
  printf("%s%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", s,
    v[0],  v[1],  v[2],  v[3],  v[4],  v[5],  v[6],  v[7],
    v[8],  v[9],  v[10], v[11], v[12], v[13], v[14], v[15],
    v[16], v[17], v[18], v[19], v[20], v[21], v[22], v[23],
    v[24], v[25], v[26], v[27], v[28], v[29], v[30], v[31]);
}

void p256_hex_u8(char const *s, __m256i in) {
  uint8_t v[32];
  memcpy(v, &in, sizeof(in));
  printf("%s%.2x %.2x %.2x %.2x | %.2x %.2x %.2x %.2x | %.2x %.2x %.2x %.2x | %.2x %.2x %.2x %.2x | %.2x %.2x %.2x %.2x | %.2x %.2x %.2x %.2x | %.2x %.2x %.2x %.2x | %.2x %.2x %.2x %.2x\n", s,
    v[0],  v[1],  v[2],  v[3],  v[4],  v[5],  v[6],  v[7],
    v[8],  v[9],  v[10], v[11], v[12], v[13], v[14], v[15],
    v[16], v[17], v[18], v[19], v[20], v[21], v[22], v[23],
    v[24], v[25], v[26], v[27], v[28], v[29], v[30], v[31]);
}

void p128_hex_u8(char const *s, __m128i in) {
  uint8_t v[16];
  memcpy(v, &in, sizeof(in));
  printf("%s%.2x %.2x %.2x %.2x | %.2x %.2x %.2x %.2x | %.2x %.2x %.2x %.2x | %.2x %.2x %.2x %.2x\n", s,
    v[0],  v[1],  v[2],  v[3],  v[4],  v[5],  v[6],  v[7],
    v[8],  v[9],  v[10], v[11], v[12], v[13], v[14], v[15]);
}

void p256_hex_u16(char const *s, __m256i in) {
  uint16_t v[16];
  memcpy(v, &in, sizeof(in));
  printf("%s%.4x %.4x %.4x %.4x | %.4x %.4x %.4x %.4x | %.4x %.4x %.4x %.4x | %.4x %.4x %.4x %.4x\n", s,
    v[0],  v[1],  v[2],  v[3],  v[4],  v[5],  v[6],  v[7],
    v[8],  v[9],  v[10], v[11], v[12], v[13], v[14], v[15]);
}

void p256_hex_u32(char const *s, __m256i in) {
  _Alignas(32) uint32_t v[8];
  memcpy(v, &in, sizeof(in));
  printf("%s%.8x %.8x %.8x %.8x %.8x %.8x %.8x %.8x\n", s,
         v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7]);
}

void p256_hex_u64(char const *s, __m256i in) {
  _Alignas(32) uint64_t v[4];
  memcpy(v, &in, sizeof(in));
  printf("%s%.16" PRIx64 " %.16" PRIx64 " %.16" PRIx64 " %.16" PRIx64 "\n",
         s, v[0], v[1], v[2], v[3]);
}
