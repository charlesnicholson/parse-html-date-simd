#include <immintrin.h>
#include <cstdio>
#include <cstring>
#include <cinttypes>
#include <ctime>

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


void p256_hex_u32(char const *s, __m256i in) {
  alignas(32) uint32_t v[8];
  memcpy(v, &in, sizeof(in));
  printf("%s%.8x %.8x %.8x %.8x %.8x %.8x %.8x %.8x\n", s,
    v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7]);
}

void p256_hex_u64(char const *s, __m256i in) {
  alignas(32) uint64_t v[4];
  memcpy(v, &in, sizeof(in));
  printf("%s%.16" PRIx64 " %.16" PRIx64 " %.16" PRIx64 " %.16" PRIx64 "\n", s, v[0], v[1], v[2], v[3]);
}

alignas(32) static char s_months1_str[] = "Jan Feb Mar Apr May Jun Jul Aug ";
alignas(32) static char s_months2_str[] = "Sep Oct Nov Dec XXXXXXXXXXXXXXXX";
int8_t const zm = -127;

void date_from_str(char const *s, struct tm *out_tm) {
  __m256i const date = _mm256_load_si256(reinterpret_cast<__m256i const*>(s));
  __m256i const months1 = _mm256_load_si256(reinterpret_cast<__m256i const*>(s_months1_str));
  __m256i const months2 = _mm256_load_si256(reinterpret_cast<__m256i const*>(s_months2_str));
  __m256i const month_splat =
    _mm256_permutevar8x32_epi32(date, _mm256_set_epi32(2, 2, 2, 2, 2, 2, 2, 2));
  __m256i const m1_sel = _mm256_sub_epi32(month_splat, months1);
  __m256i const m2_sel = _mm256_sub_epi32(month_splat, months2);
  __m256i const u32_8 = _mm256_set_epi32(8, 8, 8, 8, 8, 8, 8, 8);
  __m256i const u32_16 = _mm256_set_epi32(16, 16, 16, 16, 16, 16, 16, 16);
  __m256i const m1_sum16 = _mm256_adds_epu16(m1_sel, _mm256_srlv_epi32(m1_sel, u32_16));
  __m256i const m1_sum8 = _mm256_adds_epu16(m1_sum16, _mm256_srlv_epi32(m1_sum16, u32_8));
  __m256i const m2_sum16 = _mm256_adds_epu16(m2_sel, _mm256_srlv_epi32(m2_sel, u32_16));
  __m256i const m2_sum8 = _mm256_adds_epu16(m2_sum16, _mm256_srlv_epi32(m2_sum16, u32_8));

  __m256i const m1_0_32 =
    _mm256_shuffle_epi8(m1_sum8, _mm256_set_epi8(zm, zm, zm, zm, zm, zm, zm, zm,
                                                 zm, zm, zm, zm, 12,  8,  4,  0,
                                                 zm, zm, zm, zm, zm, zm, zm, zm,
                                                 zm, zm, zm, zm, 12,  8,  4,  0));

  __m256i const m2_32_64 =
    _mm256_shuffle_epi8(m2_sum8, _mm256_set_epi8(zm, zm, zm, zm, zm, zm, zm, zm,
                                                 12,  8,  4,  0, zm, zm, zm, zm,
                                                 zm, zm, zm, zm, zm, zm, zm, zm,
                                                 12,  8,  4,  0, zm, zm, zm, zm));

  __m256i const m1_2_bytes = // Compress month bytes into 0-16
    _mm256_permutevar8x32_epi32(_mm256_add_epi32(m1_0_32, m2_32_64),
                                _mm256_set_epi32(7, 7, 7, 7, 5, 1, 4, 0));

  __m256i const ms_bytes_00_FF = _mm256_cmpeq_epi8(m1_2_bytes, _mm256_set1_epi64x(0));
  int const month_bits = _mm_movemask_epi8(_mm256_castsi256_si128(ms_bytes_00_FF));
  out_tm->tm_mday = 32 - __builtin_clz(month_bits);

  //p256_chars(  "date:       ", date);
  //p256_chars(  "months1:    ", months1);
  //p256_chars(  "months2:    ", months2);
  //p256_chars(  "splat:      ", month_splat);
  //p256_hex_u8( "m1_sel:     ", m1_sel);
  //p256_hex_u8( "m1_sum8:    ", m1_sum8);
  //p256_hex_u8( "m1_0_32:    ", m1_0_32);
  //p256_hex_u8( "m2_sel:     ", m2_sel);
  //p256_hex_u8( "m2_sum8:    ", m2_sum8);
  //p256_hex_u8( "m2_32_64:   ", m2_32_64);
  //p256_hex_u8( "m1_2_bytes: ", m1_2_bytes);
  //p256_hex_u64( "months 0/1: ", ms_bytes_00_FF);
  //printf("mday_lo: %i\n", mday_lo);
  //printf("mday_hi: %i\n", mday_hi);
  //printf("low: 0x%.16" PRIx64 "\n", ms_bytes_64_lo);
  //printf("hi:  0x%.16" PRIx64 "\n", ms_bytes_64_hi);
}

int main(int, char const *[]) {
  alignas(32) char date_str1[32] = "Mon, 09 Mar 2020 08:13:24 GMT";
  alignas(32) char date_str2[32] = "Mon, 13 Dec 2021 08:13:24 GMT";

  struct tm dt;
  date_from_str(date_str1, &dt);
  printf("tm.tm_mday: %d\n", dt.tm_mday);

  date_from_str(date_str2, &dt);
  printf("tm.tm_mday: %d\n", dt.tm_mday);

  return 0;
}
