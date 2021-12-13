#pragma once

#include <stdbool.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

bool parse_html_date(char const *str, struct tm *out_tm);

#ifdef __cplusplus
}
#endif
