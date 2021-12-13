#include "parse_html_date.h"
#include <immintrin.h>

#if 0
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
  alignas(32) uint32_t v[8];
  memcpy(v, &in, sizeof(in));
  printf("%s%.8x %.8x %.8x %.8x %.8x %.8x %.8x %.8x\n", s,
         v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7]);
}

void p256_hex_u64(char const *s, __m256i in) {
  alignas(32) uint64_t v[4];
  memcpy(v, &in, sizeof(in));
  printf("%s%.16" PRIx64 " %.16" PRIx64 " %.16" PRIx64 " %.16" PRIx64 "\n",
         s, v[0], v[1], v[2], v[3]);
}
#endif

_Alignas(32) static char s_m1_str[] = "Jan Feb Mar Apr May Jun Jul Aug ";
_Alignas(32) static char s_m2_str[] = "Sep Oct Nov Dec XXXXXXXXXXXXXXXX";
int8_t const zm = -127;

bool parse_html_date(char const *str, struct tm *out_tm) {
  __m256i const date = _mm256_loadu_si256((__m256i const*)str);

  // To parse the 3-letter month abbreviation, load up two other 256i values.
  // 8 4-byte month abbreviations (3 chars plus space) fit per 256i.
  __m256i const m1 = _mm256_load_si256((__m256i const*)s_m1_str);
  __m256i const m2 = _mm256_load_si256((__m256i const*)s_m2_str);

  // Whatever month was provided, splat it across the 256i:
  // e.g. "Aug " -> "Aug Aug Aug Aug Aug Aug Aug Aug "
  __m256i const m_splat = _mm256_permutevar8x32_epi32(date,
    _mm256_set_epi32(2, 2, 2, 2, 2, 2, 2, 2));

  // Create a run of 0x00 0x00 0x00 0x00 by subtracting both candidate month strings
  // from the splatted string.
  __m256i const m1_sel = _mm256_sub_epi32(m_splat, m1);
  __m256i const m2_sel = _mm256_sub_epi32(m_splat, m2);

  // Somewhere in m1_sel or m2_sel, there's a run of 4x 0x00 bytes. Find it.
  // For each 4-byte run of A B C D, saturated-add them all into the run's zeroth byte.
  // First, compute the partial sum by shifting 16 and adding: A B C D -> A+C B+D C D
  // Then, compute the full sum by shifting 8 and adding: A+C B+D C D -> A+C+B+D B+D C D
  // Saturated adds with clamp the lead byte to 0XFF, so the only way 0x00 is possible
  // is if all 4 bytes are 0x00 == the month run we're looking for.

  __m256i const u32_8 = _mm256_set_epi32(8, 8, 8, 8, 8, 8, 8, 8);
  __m256i const u32_16 = _mm256_set_epi32(16, 16, 16, 16, 16, 16, 16, 16);
  __m256i const m1_sum16 = _mm256_adds_epu16(m1_sel, _mm256_srlv_epi32(m1_sel, u32_16));
  __m256i const m1_sum8 = _mm256_adds_epu16(m1_sum16, _mm256_srlv_epi32(m1_sum16, u32_8));
  __m256i const m2_sum16 = _mm256_adds_epu16(m2_sel, _mm256_srlv_epi32(m2_sel, u32_16));
  __m256i const m2_sum8 = _mm256_adds_epu16(m2_sum16, _mm256_srlv_epi32(m2_sum16, u32_8));

  // Every fourth byte (starting at 0) is nonzero except for one.
  // Shuffle them into the four lowest bytes per lane (AVX256 shuffle can't cross lanes)
  __m256i const m1_0_32 =
    _mm256_shuffle_epi8(m1_sum8, _mm256_set_epi8(zm, zm, zm, zm, zm, zm, zm, zm,
                                                 zm, zm, zm, zm, 12,  8,  4,  0,
                                                 zm, zm, zm, zm, zm, zm, zm, zm,
                                                 zm, zm, zm, zm, 12,  8,  4,  0));

  // Shuffle the second month search results into bytes 5-8 per lane.
  __m256i const m2_32_64 =
    _mm256_shuffle_epi8(m2_sum8, _mm256_set_epi8(zm, zm, zm, zm, zm, zm, zm, zm,
                                                 12,  8,  4,  0, zm, zm, zm, zm,
                                                 zm, zm, zm, zm, zm, zm, zm, zm,
                                                 12,  8,  4,  0, zm, zm, zm, zm));

  // Add the vectors and load the search results into bytes 0-15 of the search vector.
  __m256i const m1_2_bytes =
    _mm256_permutevar8x32_epi32(_mm256_add_epi32(m1_0_32, m2_32_64),
                                _mm256_set_epi32(7, 7, 7, 7, 5, 1, 4, 0));

  // Compare the search vector against 0: The "month" byte becomes 0xFF, others 0x00.
  __m256i const ms_bytes_00_FF = _mm256_cmpeq_epi8(m1_2_bytes, _mm256_set1_epi64x(0));

  // Collect the high bit of each of the 16 candidates into a scalar int.
  int const month_bits = _mm_movemask_epi8(_mm256_castsi256_si128(ms_bytes_00_FF));

  // The first bit set from the bottom is the index of the month.
  out_tm->tm_mon = 32 - __builtin_clz(month_bits);

  // Parse the numbers out of the ASCII string.
  // Index: 0123456789abcdef0123456789abcdef
  // Text:  Mon, 13 Dec 2021 08:13:24 GMT

  // Load each ascii number byte into its own u16, minding lane crossings.
  __m256i const num_chars =
    _mm256_shuffle_epi8(date, _mm256_set_epi8(zm,  zm, zm,  zm, zm, 0x8, zm, 0x7,
                                              zm, 0x5, zm, 0x4, zm, 0x2, zm, 0x1,
                                              zm,  zm, zm,  zm, zm, 0xF, zm, 0xE,
                                              zm, 0xD, zm, 0xC, zm, 0x6, zm, 0x5));

  // Subtract ASCII '0' to convert from ASCII to the actual number.
  __m256i const nums =
    _mm256_sub_epi8(num_chars, _mm256_set_epi8(0,   0, 0,   0, 0, '0', 0, '0',
                                               0, '0', 0, '0', 0, '0', 0, '0',
                                               0,   0, 0,   0, 0, '0', 0, '0',
                                               0, '0', 0, '0', 0, '0', 0, '0'));

  // Each u16 holds a digit in the 1's, 10's, 100's, or 1000's place. Multiply them up.
  __m256i const bases =
    _mm256_mullo_epi16(nums, _mm256_set_epi16(0, 0, 1, 10,   1,   10, 1, 10,
                                              0, 0, 1, 10, 100, 1000, 1, 10));

  // Add each adjacent u16 set together to combine most of the numbers.
  // Year is still split into "1000*millenium + 100*century" and "10*decade + 1*year"
  __m256i const hadd_1 = _mm256_hadd_epi16(bases, bases);

  // The numbers are still split across 128 lanes, coalesce the low u64 of each lane.
  __m256i const hadd_sums_0_127 = _mm256_permute4x64_epi64(hadd_1, 2);

  // Extract the result 128 into an array of uint16_ts for per-element extraction.
  _Alignas(16) uint16_t u16s[16];
  _mm_store_si128((__m128i*)u16s, _mm256_castsi256_si128(hadd_sums_0_127));
  out_tm->tm_hour = u16s[0];
  out_tm->tm_min = u16s[1];
  out_tm->tm_sec = u16s[2];
  out_tm->tm_mday = u16s[4];
  out_tm->tm_year = u16s[5] + u16s[6]; // Sum up the year pair.
  return true;
}
