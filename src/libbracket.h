#ifndef LIBBRACKET_H
#define LIBBRACKET_H

#ifndef LIBBRACKET_INCLUDE_NOTHING
#include <stddef.h>
#include <stdint.h>
#endif

// defs

#ifndef NAN
#define NAN (__builtin_nanf(""))
#endif

#ifndef INFINITY
#define INFINITY (__builtin_inff())
#endif

#ifndef NULL
#define NULL ((void*) 0)
#endif

#ifndef EOF
#define EOF (-1)
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

#ifndef ULONG_MAX
#define ULONG_MAX (~(unsigned long)0)
#endif

#ifndef LONG_MAX
#define LONG_MAX (ULONG_MAX >> 1)
#endif

#ifndef LONG_MIN
#define LONG_MIN (-LONG_MAX - 1)
#endif

#ifndef ULLONG_MAX
#define ULLONG_MAX (~(unsigned long long)0)
#endif

#ifndef LLONG_MAX
#define LLONG_MAX (ULLONG_MAX >> 1)
#endif

#ifndef LLONG_MIN
#define LLONG_MIN (-LLONG_MAX - 1)
#endif

#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif

#ifndef UCHAR_MAX
#define UCHAR_MAX 255
#endif

// math

// NOTE: math functions here should not raise any kind of exception or modify
// errno, since those require additional features not assumed in libbracket.

#define isnan(x) __builtin_isnan(x)

double fmax(double x, double y);
float fmaxf(float x, float y);
long double fmaxl(long double x, long double y);

double floor(double x);
double fabs(double x);
double copysign(double x, double y);
long double copysignl(long double x, long double y);

double frexp(double x, int *out);

double sqrt(double x);
double pow(double x, double y);
double fmod(double x, double y);

// TODO: make this full-precision
static inline long double fmodl(long double x, long double y) {
	return fmod((double) x, (double) y);
}

double scalbn(double x, int exp);

static inline long double scalbnl(long double x, int exp) {
	return scalbn((double) x, exp);
}

double ldexp(double value, int exp);
int finite(double x);

union bracket_split_double {
	double asfull;
	struct {
		int low;
		int high;
	};
};

#define bracket_HI(x) (((union bracket_split_double) { .asfull = x }).high)
#define bracket_LO(x) (((union bracket_split_double) { .asfull = x }).low)
#define bracket_C(l, h) (((union bracket_split_double) { .low = l, .high = h }).asfull)
#define bracket_uHI(x, n) (bracket_C(bracket_LO(x), n))
#define bracket_uLO(x, n) (bracket_C(n, bracket_HI(x)))

// parsing and encoding

struct __floatscan_state {
	const char *buf;
	const char *buf_start;
};
long double __floatscan(struct __floatscan_state *, int, int);

// memory

int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);

const char *strstr(const char *haystack, const char *needle);
const char *strchr(const char *s, int c);

size_t strlen(const char *s);

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);

#endif
