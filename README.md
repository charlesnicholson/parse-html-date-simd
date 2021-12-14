# parse-html-date-simd
AVX256 HTML Date Header parser

Parses well-formed Date strings a la https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Date.
It will give you absolute garbage if the string is not well-formed exactly as described above.

Please don't actually use this as-is; it's really just a toy for me to learn AVX256 :)

Here's some pretty assembly to look at: https://gcc.godbolt.org/z/hbbThEes3

I found https://www.officedaytime.com/simd512e/ to be an extremely helpful resource!
