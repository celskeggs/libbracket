#ifndef LIBBRACKET_H
#define LIBBRACKET_H

#ifndef LIBBRACKET_INCLUDE_NOTHING
#include <stddef.h>
#include <stdint.h>
#endif

// defs

#ifndef NULL
#define NULL ((void*) 0)
#endif

#ifndef UINT_MAX
#define UINT_MAX (~(unsigned int)0)
#endif

#ifndef INT_MAX
#define INT_MAX (UINT_MAX >> 1)
#endif

#ifndef INT_MIN
#define INT_MIN (-INT_MAX - 1)
#endif

#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif

// math

#define isnan(x) __builtin_isnan(x)

double fmax(double x, double y);
float fmaxf(float x, float y);
long double fmaxl(long double x, long double y);

double frexp(double x, int *out);

// memory

int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);

const char *strstr(const char *haystack, const char *needle);
const char *strchr(const char *s, int c);

size_t strlen(const char *s);

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);

#endif
