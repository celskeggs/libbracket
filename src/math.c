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

// little-endian
#define __HI(x) *(1+(int*)&x)
#define __LO(x) *(int*)&x
#define __HIp(x) *(1+(int*)x)
#define __LOp(x) *(int*)x

union xdouble {
	double asfull;
	struct {
		int low;
		int high;
	};
};

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
	union xdouble xd = { .asfull = x };
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
