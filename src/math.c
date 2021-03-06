#include "libbracket.h"

double fmax(double x, double y) {
	return x >= y || isnan(y) ? x : y;
}

float fmaxf(float x, float y) {
	return x >= y || isnan(y) ? x : y;
}

long double fmaxl(long double x, long double y) {
	return x >= y || isnan(y) ? x : y;
}

// The following code is from musl

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

long double frexpl(long double x, int *e)
{
        union bracket_ldshape u = {x};
        int ee = u.i.se & 0x7fff;

        if (!ee) {
                if (x) {
                        x = frexpl(x*0x1p120, e);
                        *e -= 120;
                } else *e = 0;
                return x;
        } else if (ee == 0x7fff) {
                return x;
        }

        *e = ee - 0x3ffe;
        u.i.se &= 0x8000;
        u.i.se |= 0x3ffe;
        return u.f;
}

// The following code is from fdlibm. See below copyright:
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

/*
 * for non-zero x 
 *	x = frexp(arg,&exp);
 * return a double fp quantity x such that 0.5 <= |x| <1.0
 * and the corresponding binary exponent "exp". That is
 *	arg = x*2^exp.
 * If arg is inf, 0.0, or NaN, then frexp(arg,&exp) returns arg 
 * with *exp=0. 
 */

static const double two54 =  1.80143985094819840000e+16; /* 0x43500000, 0x00000000 */

double frexp(double x, int *eptr) {
	int ix;
	union bracket_split_double xd = { .asfull = x };
	ix = 0x7fffffff & xd.high;
	*eptr = 0;
	if (ix >= 0x7ff00000 || ((ix|xd.low) == 0)) {
		return xd.asfull;	/* 0,inf,nan */
	}
	if (ix < 0x00100000) {		/* subnormal */
	    xd.asfull *= two54;
	    ix = xd.high & 0x7fffffff;
	    *eptr = -54;
	}
	*eptr += (ix>>20) - 1022;
	xd.high = (xd.high & 0x800fffff) | 0x3fe00000;
	return xd.asfull;
}

double fabs(double x) {
	return bracket_uHI(x, bracket_HI(x) & 0x7FFFFFFF);
}

_Static_assert(sizeof(double) == sizeof(int) * 2, "double check");
_Static_assert(sizeof(long double) == sizeof(int) * 4, "double check 2");

double copysign(double x, double y) {
	return bracket_uHI(x, (bracket_HI(x)&0x7fffffff)|(bracket_HI(y)&0x80000000));
}

long double copysignl(long double x, long double y) {
	int *xp = (int*) &x, *yp = (int*) &y;
	xp[3] &= 0x7FFFFFFF;
	xp[3] |= 0x80000000 & yp[3];
	return x;
}

double ldexp(double value, int exp) {
	if(!finite(value)||value==0.0) return value;
	value = scalbn(value,exp);
	return value;
}

int finite(double x) {
	int hx;
	hx = bracket_HI(x);
	return (unsigned)((hx&0x7fffffff)-0x7ff00000)>>31;
}
