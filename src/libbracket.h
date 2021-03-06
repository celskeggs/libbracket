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

#ifndef USHRT_MAX
#define USHRT_MAX (65535)
#endif

#ifndef SHRT_MAX
#define SHRT_MAX (32767)
#endif

#ifndef SHRT_MIN
#define SHRT_MIN (-32768)
#endif

#ifndef UINT_MAX
#define UINT_MAX (~(unsigned int)0)
#endif

#ifndef INT_MAX
#define INT_MAX ((int)(UINT_MAX >> 1))
#endif

#ifndef INT_MIN
#define INT_MIN ((int)(-INT_MAX - 1))
#endif

#ifndef ULONG_MAX
#define ULONG_MAX (~(unsigned long)0)
#endif

#ifndef LONG_MAX
#define LONG_MAX ((long)(ULONG_MAX >> 1))
#endif

#ifndef LONG_MIN
#define LONG_MIN ((long)(-LONG_MAX - 1))
#endif

#ifndef ULLONG_MAX
#define ULLONG_MAX (~(unsigned long long)0)
#endif

#ifndef LLONG_MAX
#define LLONG_MAX ((long long)(ULLONG_MAX >> 1))
#endif

#ifndef LLONG_MIN
#define LLONG_MIN ((long long)(-LLONG_MAX - 1))
#endif

#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif

#ifndef UCHAR_MAX
#define UCHAR_MAX 255
#endif

// math

_Static_assert(sizeof(long double) == 16, "wrong size for long double");
_Static_assert(sizeof(double) == 8, "wrong size for double");
_Static_assert(sizeof(float) == 4, "wrong size for float");

#define LDBL_TRUE_MIN 3.6451995318824746025e-4951L
#define LDBL_MIN     3.3621031431120935063e-4932L
#define LDBL_MAX     1.1897314953572317650e+4932L
#define LDBL_EPSILON 1.0842021724855044340e-19L

#define LDBL_MANT_DIG 64
#define LDBL_MIN_EXP (-16381)
#define LDBL_MAX_EXP 16384

#define LDBL_DIG 18
#define LDBL_MIN_10_EXP (-4931)
#define LDBL_MAX_10_EXP 4932

#define DBL_MANT_DIG 53
#define DBL_MIN_EXP (-1021)
#define DBL_MAX_EXP 1024
#define DBL_HAS_SUBNORM 1

#define FLT_MANT_DIG 24
#define FLT_MIN_EXP (-125)
#define FLT_MAX_EXP 128
#define FLT_HAS_SUBNORM 1

// NOTE: math functions here should not raise any kind of exception or modify
// errno, since those require additional features not assumed in libbracket.

#define isnan(x) __builtin_isnan(x)

static inline int abs(int x) { return x < 0 ? -x : x; }

double fmax(double x, double y);
float fmaxf(float x, float y);
long double fmaxl(long double x, long double y);

double floor(double x);
double fabs(double x);
double copysign(double x, double y);
long double copysignl(long double x, long double y);

double frexp(double x, int *out);
long double frexpl(long double x, int *e);

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

// code from musl (see licenses in other files)

#define FP_NAN       0
#define FP_INFINITE  1
#define FP_ZERO      2
#define FP_SUBNORMAL 3
#define FP_NORMAL    4

union bracket_ldshape {
        long double f;
        struct {
                uint64_t m;
                uint16_t se;
        } i;
};

static inline int __fpclassifyl(long double x)
{
        union bracket_ldshape u = {x};
        int e = u.i.se & 0x7fff;
        int msb = u.i.m>>63;
        if (!e && !msb)
                return u.i.m ? FP_SUBNORMAL : FP_ZERO;
        if (!msb)
                return FP_NAN;
        if (e == 0x7fff)
                return u.i.m << 1 ? FP_NAN : FP_INFINITE;
        return FP_NORMAL;
}

static inline unsigned __FLOAT_BITS(float __f)
{
        union {float __f; unsigned __i;} __u;
        __u.__f = __f;
        return __u.__i;
}

static inline unsigned long long __DOUBLE_BITS(double __f)
{
        union {double __f; unsigned long long __i;} __u;
        __u.__f = __f;
        return __u.__i;
}

int __fpclassifyl(long double);

#define signbit(x) ( \
        sizeof(x) == sizeof(float) ? (int)(__FLOAT_BITS(x)>>31) : \
        sizeof(x) == sizeof(double) ? (int)(__DOUBLE_BITS(x)>>63) : \
        (int) (((union bracket_ldshape) {.f = (long double) x}).i.se >> 15) )

#define isfinite(x) ( \
        sizeof(x) == sizeof(float) ? (__FLOAT_BITS(x) & 0x7fffffff) < 0x7f800000 : \
        sizeof(x) == sizeof(double) ? (__DOUBLE_BITS(x) & -1ULL>>1) < 0x7ffULL<<52 : \
        __fpclassifyl(x) > FP_INFINITE)

// parsing and encoding

struct __floatscan_state {
	const char *buf;
	const char *buf_start;
};
long double __floatscan(struct __floatscan_state *, int, int);

double strtod(const char *restrict s, char **restrict p);
float strtof(const char *restrict s, char **restrict p);
long double strtold(const char *restrict s, char **restrict p);

#define INTBUFSIZE 24

int itostr(int64_t value, char *output, size_t len);
int utostr(uint64_t value, char *output, size_t len);
int dtostr(long double value, char *output, size_t len);

// memory

int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);

const char *strstr(const char *haystack, const char *needle);
const char *strchr(const char *s, int c);
size_t strspn(const char *haystack, const char *accept);
size_t strcspn(const char *haystack, const char *reject);
const char *strpbrk(const char *s, const char *accept);

size_t strlen(const char *s);

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

// chars

int toupper(int c);
int tolower(int c);
int isalpha(int c);
int isdigit(int c);
int isalnum(int c);

#endif
