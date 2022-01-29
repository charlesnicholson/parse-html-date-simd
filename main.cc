#include "parse_html_date.h"

#include <cstdio>
#include <cstdint>
#include <ctime>

#include <mach/mach_time.h>

void print_tm(const struct tm *dt) {
  printf("%d/%d/%d %.2d:%.2d:%.2d\n",
    dt->tm_mon, dt->tm_mday, dt->tm_year, dt->tm_hour, dt->tm_min, dt->tm_sec);
}

int main(int, char const *[]) {
  char const *date_str1 = "Mon, 09 Mar 2020 08:13:24 GMT";
  char const *date_str2 = "Tue, 13 Dec 2021 23:59:59 GMT";
  char const *date_str3 = "Wed, 20 Aug 2022 12:34:56 GMT";
  char const *date_str4 = "Thu, 30 Jan 2023 22:00:11 GMT";

  struct tm dt;
  parse_html_date(date_str1, &dt);
  print_tm(&dt);
  parse_html_date(date_str2, &dt);
  print_tm(&dt);
  parse_html_date(date_str3, &dt);
  print_tm(&dt);
  parse_html_date(date_str4, &dt);
  print_tm(&dt);

  mach_timebase_info_data_t rate_nsec;
  mach_timebase_info(&rate_nsec);

  uint64_t const rate = 1000000000LL * rate_nsec.numer / rate_nsec.denom;
  auto const iter = 1'000'000'000;

  uint64_t const start = mach_absolute_time(); // Begin profiling
  for (auto i = 0; i < iter; ++i) {
    parse_html_date(date_str1, &dt);
    parse_html_date(date_str2, &dt);
    parse_html_date(date_str3, &dt);
    parse_html_date(date_str4, &dt);
  }
  uint64_t const end = mach_absolute_time(); // End profiling

  double const elapsed = double(end - start) / double(rate);
  double const cost_s = elapsed / (double)iter / 4.;
  double const cost_ns = cost_s * 1e9;

  printf("%fs elapsed, %gns per call\n", elapsed, cost_ns);
  return 0;
}
