#ifndef RANDOMTEST_COMMON_H
#define RANDOMTEST_COMMON_H

#define BUFSIZE 2048
#define MAXFRAMES 20
#define SKIP_FRAMES 4

/** Returns pointer to last byte with '\0' */
char *url_encode(const char* prefix, const char *str, char* out, const char* end);

/** sends passed rawStacktrace to default crash report collector */
void send_stacktrace_by_curl(const char* rawStacktrace);

#endif
