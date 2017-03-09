#include "libbracket.h"
// from musl

// ----------------------------------------------------------------------
// Copyright © 2005-2014 Rich Felker, et al.
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// ----------------------------------------------------------------------

/* Do not override this check. The floating point printing code below
 * depends on the float.h constants being right. If they are wrong, it
 * may overflow the stack. */
#if LDBL_MANT_DIG == 53
typedef char compiler_defines_long_double_incorrectly[9-(int)sizeof(long double)];
#endif

// todo: make this smaller, because this seems overly cautious.
#define MIN_PRINT_SIZE 128

static char *fmt_u(uintmax_t x, char *s)
{
        unsigned long y;
        for (   ; x>ULONG_MAX; x/=10) *--s = '0' + x%10;
        for (y=x;           y; y/=10) *--s = '0' + y%10;
        return s;
}

static void pad(char *bufout, int *boi, char c, int w, int l, int fl)
{
        char pad[256];
        if (l >= w) return;
        l = w - l;
        memset(pad, c, l>sizeof pad ? sizeof pad : l);
        for (; l >= sizeof pad; l -= sizeof pad) {
		memcpy(bufout + *boi, pad, sizeof(pad));
		*boi += sizeof(pad);
	}
	memcpy(bufout + *boi, pad, l);
	*boi += l;
}

static int dtostr_i(long double y, char bufout[MIN_PRINT_SIZE]) {
	int boi = 0;
	uint32_t big[(LDBL_MANT_DIG+28)/29 + 1          // mantissa expansion
		+ (LDBL_MAX_EXP+LDBL_MANT_DIG+28+8)/9]; // exponent expansion
	uint32_t *a, *d, *r, *z;
	int e2=0, e, i, j, l;
	char buf[9+LDBL_MANT_DIG/4];
	const char *prefix="-0X+0X 0X-0x+0x 0x";
	int pl;

	pl=1;
	if (signbit(y)) {
		y=-y;
	} else prefix++, pl=0;

	if (!isfinite(y)) {
		char *s = "inf";
		if (y!=y) s="nan";
		if (pl) {
			bufout[0] = '-';
		}
		memcpy(bufout + pl, s, 3);
		return pl + 3;
	}

	y = frexpl(y, &e2) * 2;
	if (y) e2--;

	int p=6;

	if (y) y *= 0x1p28, e2-=28;

	if (e2<0) a=r=z=big;
	else a=r=z=big+sizeof(big)/sizeof(*big) - LDBL_MANT_DIG - 1;

	do {
		*z = y;
		y = 1000000000*(y-*z++);
	} while (y);

	while (e2>0) {
		uint32_t carry=0;
		int sh=(e2 < 29 ? e2 : 29);
		for (d=z-1; d>=a; d--) {
			uint64_t x = ((uint64_t)*d<<sh)+carry;
			*d = x % 1000000000;
			carry = x / 1000000000;
		}
		if (carry) *--a = carry;
		while (z>a && !z[-1]) z--;
		e2-=sh;
	}
	while (e2<0) {
		uint32_t carry=0, *b;
		int sh=(-e2 < 9 ? -e2 : 9), need=1+(p+LDBL_MANT_DIG/3U+8)/9;
		for (d=a; d<z; d++) {
			uint32_t rm = *d & ((1<<sh)-1);
			*d = (*d>>sh) + carry;
			carry = (1000000000>>sh) * rm;
		}
		if (!*a) a++;
		if (carry) *z++ = carry;
		/* Avoid (slow!) computation past requested precision */
		b = r;
		if (z-b > need) z = b+need;
		e2+=sh;
	}

	if (a<z) for (i=10, e=9*(r-a); *a>=i; i*=10, e++);
	else e=0;

	/* Perform rounding: j is precision after the radix (possibly neg) */
	j = p;
	if (j < 9*(z-r-1)) {
		uint32_t x;
		/* We avoid C's broken division of negative numbers */
		d = r + 1 + ((j+9*LDBL_MAX_EXP)/9 - LDBL_MAX_EXP);
		j += 9*LDBL_MAX_EXP;
		j %= 9;
		for (i=10, j++; j<9; i*=10, j++);
		x = *d % i;
		/* Are there any significant digits past j? */
		if (x || d+1!=z) {
			long double round = 2/LDBL_EPSILON;
			long double small;
			if ((*d/i & 1) || (i==1000000000 && d>a && (d[-1]&1)))
				round += 2;
			if (x<i/2) small=0x0.8p0;
			else if (x==i/2 && d+1==z) small=0x1.0p0;
			else small=0x1.8p0;
			if (pl && *prefix=='-') round*=-1, small*=-1;
			*d -= x;
			/* Decide whether to round by probing round+small */
			if (round+small != round) {
				*d = *d + i;
				while (*d > 999999999) {
					*d--=0;
					if (d<a) *--a=0;
					(*d)++;
				}
				for (i=10, e=9*(r-a); *a>=i; i*=10, e++);
			}
		}
		if (z>d+1) z=d+1;
	}
	for (; z>a && !z[-1]; z--);
	
	if (p > INT_MAX-1-(p))
		return -1;
	l = 1 + p + (p);
	if (e > INT_MAX-l) return -1;
	if (e>0) l+=e;

	if (l > INT_MAX-pl) return -1;
	
	if (pl) {
		bufout[boi++] = '-';
	}

	if (a>r) a=r;
	for (d=a; d<=r; d++) {
		char *s = fmt_u(*d, buf+9);
		if (d!=a) while (s>buf) *--s='0';
		else if (s==buf+9) *--s='0';
		memcpy(bufout + boi, s, buf + 9 - s);
		boi += buf + 9 - s;
	}
	if (p) bufout[boi++] = '.';
	for (; d<z && p>0; d++, p-=9) {
		char *s = fmt_u(*d, buf+9);
		while (s>buf) *--s='0';
		memcpy(bufout + boi, s, (p < 9 ? p : 9));
		boi += (p < 9 ? p : 9);
	}
	pad(bufout, &boi, '0', p+9, 9, 0);

	return pl + l;
}

int dtostr(long double value, char *out, size_t len) {
	char buf[MIN_PRINT_SIZE];
	int lenr = dtostr_i(value, buf);
	if (lenr < 0) {
		__builtin_trap(); // TODO: remove this case
	}
	if (lenr > len - 1) {
		lenr = len - 1;
	}
	memcpy(out, buf, lenr);
	out[lenr] = '\0';
	return lenr;
}
