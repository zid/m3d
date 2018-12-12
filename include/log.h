#ifndef LOG_H
#define LOG_H

#ifdef __GNUC__
#define PRINTF_LIKE __attribute__ ((format (printf, 1, 2)))
#else
#define PRINTF_LIKE
#endif

void PRINTF_LIKE nlog(const char *fmt, ...);

#endif