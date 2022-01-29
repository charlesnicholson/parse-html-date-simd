#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct tm;

bool parse_html_date(char const *str, struct tm *out_tm);

#ifdef __cplusplus
}
#endif
