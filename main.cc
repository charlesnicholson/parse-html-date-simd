#include <immintrin.h>
#include <cstdio>
#include <cstring>
#include <cinttypes>

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

alignas(32) static char s_months1_str[] = "Jan Feb Mar Apr May Jun Jul Aug ";
alignas(32) static char s_months2_str[] = "Sep Oct Nov Dec XXXXXXXXXXXXXXXX";

int main(int, char const *[]) {
  alignas(32) char date_str[32] = "Mon, 09 Mar 2020 08:13:24 GMT";

  __m256i const date = _mm256_load_si256(reinterpret_cast<__m256i*>(date_str));

  __m256i const months1 = _mm256_load_si256(reinterpret_cast<__m256i*>(s_months1_str));
  __m256i const months2 = _mm256_load_si256(reinterpret_cast<__m256i*>(s_months2_str));
  __m256i const month_permute = _mm256_set_epi32(2, 2, 2, 2, 2, 2, 2, 2);
  __m256i const month_splat = _mm256_permutevar8x32_epi32(date, month_permute);
  __m256i const m1_sel = _mm256_sub_epi32(month_splat, months1);
  __m256i const m2_sel = _mm256_sub_epi32(month_splat, months2);
  __m256i const u32_8 = _mm256_set_epi32(8, 8, 8, 8, 8, 8, 8, 8);
  __m256i const u32_16 = _mm256_set_epi32(16, 16, 16, 16, 16, 16, 16, 16);
  __m256i const m1_sum16 = _mm256_adds_epu16(m1_sel, _mm256_srlv_epi32(m1_sel, u32_16));
  __m256i const m1_sum8 = _mm256_adds_epu16(m1_sum16, _mm256_srlv_epi32(m1_sum16, u32_8));
  __m256i const m2_sum16 = _mm256_adds_epu16(m2_sel, _mm256_srlv_epi32(m2_sel, u32_16));
  __m256i const m2_sum8 = _mm256_adds_epu16(m2_sum16, _mm256_srlv_epi32(m2_sum16, u32_8));

  int8_t const zm = -127;

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

  __m256i const ms_bytes = // each month gets a byte, the current month is 0x00
    _mm256_permutevar8x32_epi32(_mm256_add_epi32(m1_0_32, m2_32_64),
                                _mm256_set_epi32(7, 7, 7, 7, 5, 4, 1, 0));

  __m256i const ms_bytes_00_FF = _mm256_cmpeq_epi8(ms_bytes, _mm256_set1_epi64x(0));
  __m128i const ms_bytes_00_FF_128 = _mm256_castsi256_si128(ms_bytes_00_FF);

  int64_t const ms_bytes_64_low = _mm_extract_epi64(ms_bytes_00_FF_128, 0);
  int64_t const ms_bytes_64_hi = _mm_extract_epi64(ms_bytes_00_FF_128, 1);
  int const index_low = ms_bytes_64_low ? __builtin_clzll(ms_bytes_64_low) : 0;
  int const index_hi = ms_bytes_64_hi ? __builtin_clzll(ms_bytes_64_hi) : 0;

  p256_chars(  "date:       ", date);
  p256_chars(  "months1:    ", months1);
  p256_chars(  "months2:    ", months2);
  p256_chars(  "splat:      ", month_splat);
//  p256_hex_u8( "months:     ", ms_bytes);
  p256_hex_u8( "months 0/1: ", ms_bytes_00_FF);
  printf("low_index: %i\n", index_low);
  printf("hi_index: %i\n", index_hi);
  printf("low: 0x%.16" PRIx64 "\n", ms_bytes_64_low);
  printf("hi:  0x%.16" PRIx64 "\n", ms_bytes_64_hi);
  return 0;
}
