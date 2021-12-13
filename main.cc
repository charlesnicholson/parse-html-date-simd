#include "parse_html_date.h"
#include <cstdio>

void print_tm(const struct tm *dt) {
  printf("%d/%d/%d %d:%d:%d\n",
    dt->tm_mon, dt->tm_mday, dt->tm_year, dt->tm_hour, dt->tm_min, dt->tm_sec);
}

int main(int, char const *[]) {
  char const *date_str1 = "Mon, 09 Mar 2020 08:13:24 GMT";
  char const *date_str2 = "Mon, 13 Dec 2021 23:59:59 GMT";

  struct tm dt;
  parse_html_date(date_str1, &dt);
  print_tm(&dt);

  parse_html_date(date_str2, &dt);
  print_tm(&dt);
  return 0;
}
