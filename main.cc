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

alignas(32) static char s_m1_str[] = "Jan Feb Mar Apr May Jun Jul Aug ";
alignas(32) static char s_m2_str[] = "Sep Oct Nov Dec XXXXXXXXXXXXXXXX";
int8_t const zm = -127;

void date_from_str(char const *s, struct tm *out_tm) {
  __m256i const date = _mm256_load_si256(reinterpret_cast<__m256i const*>(s));
  __m256i const m1 = _mm256_load_si256(reinterpret_cast<__m256i const*>(s_m1_str));
  __m256i const m2 = _mm256_load_si256(reinterpret_cast<__m256i const*>(s_m2_str));
  __m256i const m_splat = _mm256_permutevar8x32_epi32(date, _mm256_set_epi32(2, 2, 2, 2, 2, 2, 2, 2));
  __m256i const m1_sel = _mm256_sub_epi32(m_splat, m1);
  __m256i const m2_sel = _mm256_sub_epi32(m_splat, m2);
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
  out_tm->tm_mon = 32 - __builtin_clz(month_bits);

  // 0123456789abcdef0123456789abcdef
  // Mon, 13 Dec 2021 08:13:24 GMT
  // 103020002010    008010302040

  __m256i const num_chars =
    _mm256_shuffle_epi8(date, _mm256_set_epi8(zm,  zm, zm,  zm, zm, 0x8, zm, 0x7,
                                              zm, 0x5, zm, 0x4, zm, 0x2, zm, 0x1,
                                              zm,  zm, zm,  zm, zm, 0xF, zm, 0xE,
                                              zm, 0xD, zm, 0xC, zm, 0x6, zm, 0x5));
  __m256i const nums =
    _mm256_sub_epi8(num_chars, _mm256_set_epi8(0,   0, 0,   0, 0, '0', 0, '0',
                                               0, '0', 0, '0', 0, '0', 0, '0',
                                               0,   0, 0,   0, 0, '0', 0, '0',
                                               0, '0', 0, '0', 0, '0', 0, '0'));

  __m256i const bases =
    _mm256_mullo_epi16(nums, _mm256_set_epi16(0, 0, 1, 10,   1,   10, 1, 10,
                                              0, 0, 1, 10, 100, 1000, 1, 10));

  __m256i const hadd_1 = _mm256_hadd_epi16(bases, bases);
  __m256i const hadd_sums_0_127 = _mm256_permute4x64_epi64(hadd_1, 2);

  alignas(16) uint16_t u16s[16];
  _mm_store_si128((__m128i*)u16s, _mm256_castsi256_si128(hadd_sums_0_127));
  out_tm->tm_hour = u16s[0];
  out_tm->tm_min = u16s[1];
  out_tm->tm_sec = u16s[2];
  out_tm->tm_mday = u16s[4];
  out_tm->tm_year = u16s[5] + u16s[6];
}

void print_tm(const struct tm *dt) {
  printf("%d/%d/%d %d:%d:%d\n",
    dt->tm_mon, dt->tm_mday, dt->tm_year, dt->tm_hour, dt->tm_min, dt->tm_sec);
}

int main(int, char const *[]) {
  alignas(32) char date_str1[32] = "Mon, 09 Mar 2020 08:13:24 GMT";
  alignas(32) char date_str2[32] = "Mon, 13 Dec 2021 08:13:24 GMT";

  struct tm dt;
  date_from_str(date_str1, &dt);
  print_tm(&dt);

  date_from_str(date_str2, &dt);
  print_tm(&dt);
  return 0;
}
