/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

// Modified for migui 

#pragma once
#ifndef __MATH_PRIV__
#define __MATH_PRIV__

// #if IEEE_WORD_ORDER == LITTLE_ENDIAN
typedef union
{
	double value;
	struct
	{
		uint32_t lsw;
		uint32_t msw;
	} parts;
	struct
	{
		uint64_t w;
	} xparts;
} ieee_double_shape_type;

typedef union
{
	long double value;
	struct {
		uint32_t lswlo;
		uint32_t lswhi;
		uint32_t mswlo;
		uint32_t mswhi;
	} parts32;
	struct {
		uint64_t lsw;
		uint64_t msw;
	} parts64;
} ieee_quad_shape_type;

/* Get the more significant 32 bit int from a double.  */
#define GET_HIGH_WORD(i,d)					\
do {								\
  ieee_double_shape_type gh_u;					\
  gh_u.value = (d);						\
  (i) = gh_u.parts.msw;						\
} while (0)

/* Get the less significant 32 bit int from a double.  */
#define GET_LOW_WORD(i,d)					\
do {								\
  ieee_double_shape_type gl_u;					\
  gl_u.value = (d);						\
  (i) = gl_u.parts.lsw;						\
} while (0)

/* Set the more significant 32 bits of a double from an int.  */
#define SET_HIGH_WORD(d,v)					\
do {								\
  ieee_double_shape_type sh_u;					\
  sh_u.value = (d);						\
  sh_u.parts.msw = (v);						\
  (d) = sh_u.value;						\
} while (0)

/* Set the less significant 32 bits of a double from an int.  */
#define SET_LOW_WORD(d,v)					\
do {								\
  ieee_double_shape_type sl_u;					\
  sl_u.value = (d);						\
  sl_u.parts.lsw = (v);						\
  (d) = sl_u.value;						\
} while (0)

/* Set a double from two 32 bit ints.  */
#define INSERT_WORDS(d,ix0,ix1)					\
do {								\
  ieee_double_shape_type iw_u;					\
  iw_u.parts.msw = (ix0);					\
  iw_u.parts.lsw = (ix1);					\
  (d) = iw_u.value;						\
} while (0)

/* Set a double from a 64-bit int. */
#define INSERT_WORD64(d,ix)					\
do {								\
  ieee_double_shape_type iw_u;					\
  iw_u.xparts.w = (ix);						\
  (d) = iw_u.value;						\
} while (0)

/* Get two 32 bit ints from a double.  */
#define EXTRACT_WORDS(ix0,ix1,d)				\
do {								\
  ieee_double_shape_type ew_u;					\
  ew_u.value = (d);						\
  (ix0) = ew_u.parts.msw;					\
  (ix1) = ew_u.parts.lsw;					\
} while (0)

/* Get a 64-bit int from a double. */
#define EXTRACT_WORD64(ix,d)					\
do {								\
  ieee_double_shape_type ew_u;					\
  ew_u.value = (d);						\
  (ix) = ew_u.xparts.w;						\
} while (0)

typedef union
{
	float value;
	/* FIXME: Assumes 32 bit int.  */
	unsigned int word;
} ieee_float_shape_type;

/* Get a 32 bit int from a float.  */
#define GET_FLOAT_WORD(i,d)					\
do {								\
  ieee_float_shape_type gf_u;					\
  gf_u.value = (d);						\
  (i) = gf_u.word;						\
} while (0)

/* Set a float from a 32 bit int.  */
#define SET_FLOAT_WORD(d,i)					\
do {								\
  ieee_float_shape_type sf_u;					\
  sf_u.word = (i);						\
  (d) = sf_u.value;						\
} while (0)

#ifndef INLINE_REM_PIO2
int	__ieee754_rem_pio2(double, double*);
#endif

double	__ldexp_exp(double, int);

/*
 * The rnint() family rounds to the nearest integer for a restricted range
 * range of args (up to about 2**MANT_DIG).  We assume that the current
 * rounding mode is FE_TONEAREST so that this can be done efficiently.
 * Extra precision causes more problems in practice, and we only centralize
 * this here to reduce those problems, and have not solved the efficiency
 * problems.  The exp2() family uses a more delicate version of this that
 * requires extracting bits from the intermediate value, so it is not
 * centralized here and should copy any solution of the efficiency problems.
 */

static inline double
rnint(double_t x)
{
	/*
	 * This casts to double to kill any extra precision.  This depends
	 * on the cast being applied to a double_t to avoid compiler bugs
	 * (this is a cleaner version of STRICT_ASSIGN()).  This is
	 * inefficient if there actually is extra precision, but is hard
	 * to improve on.  We use double_t in the API to minimise conversions
	 * for just calling here.  Note that we cannot easily change the
	 * magic number to the one that works directly with double_t, since
	 * the rounding precision is variable at runtime on x86 so the
	 * magic number would need to be variable.  Assuming that the
	 * rounding precision is always the default is too fragile.  This
	 * and many other complications will move when the default is
	 * changed to FP_PE.
	 */
	return ((double)(x + 0x1.8p52) - 0x1.8p52);
}

static inline float
rnintf(float_t x)
{
	/*
	 * As for rnint(), except we could just call that to handle the
	 * extra precision case, usually without losing efficiency.
	 */
	return ((float)(x + 0x1.8p23F) - 0x1.8p23F);
}

#define	STRICT_ASSIGN(type, lval, rval)	((lval) = (rval))

/*
 * Mix 0, 1 or 2 NaNs.  First add 0 to each arg.  This normally just turns
 * signaling NaNs into quiet NaNs by setting a quiet bit.  We do this
 * because we want to never return a signaling NaN, and also because we
 * don't want the quiet bit to affect the result.  Then mix the converted
 * args using the specified operation.
 *
 * When one arg is NaN, the result is typically that arg quieted.  When both
 * args are NaNs, the result is typically the quietening of the arg whose
 * mantissa is largest after quietening.  When neither arg is NaN, the
 * result may be NaN because it is indeterminate, or finite for subsequent
 * construction of a NaN as the indeterminate 0.0L/0.0L.
 *
 * Technical complications: the result in bits after rounding to the final
 * precision might depend on the runtime precision and/or on compiler
 * optimizations, especially when different register sets are used for
 * different precisions.  Try to make the result not depend on at least the
 * runtime precision by always doing the main mixing step in long double
 * precision.  Try to reduce dependencies on optimizations by adding the
 * the 0's in different precisions (unless everything is in long double
 * precision).
 */
#define	nan_mix(x, y)		(nan_mix_op((x), (y), +))
#define	nan_mix_op(x, y, op)	(((x) + 0.0L) op ((y) + 0))

#ifdef _COMPLEX_H

 /*
  * C99 specifies that complex numbers have the same representation as
  * an array of two elements, where the first element is the real part
  * and the second element is the imaginary part.
  */
typedef union {
	float complex f;
	float a[2];
} float_complex;
typedef union {
	double complex f;
	double a[2];
} double_complex;
typedef union {
	long double complex f;
	long double a[2];
} long_double_complex;
#define	REALPART(z)	((z).a[0])
#define	IMAGPART(z)	((z).a[1])

/*
 * Inline functions that can be used to construct complex values.
 *
 * The C99 standard intends x+I*y to be used for this, but x+I*y is
 * currently unusable in general since gcc introduces many overflow,
 * underflow, sign and efficiency bugs by rewriting I*y as
 * (0.0+I)*(y+0.0*I) and laboriously computing the full complex product.
 * In particular, I*Inf is corrupted to NaN+I*Inf, and I*-0 is corrupted
 * to -0.0+I*0.0.
 *
 * The C11 standard introduced the macros CMPLX(), CMPLXF() and CMPLXL()
 * to construct complex values.  Compilers that conform to the C99
 * standard require the following functions to avoid the above issues.
 */

#ifndef CMPLXF
static __inline float_complex
CMPLXF(float x, float y)
{
	float_complex z;

	REALPART(z) = x;
	IMAGPART(z) = y;
	//return (z.f);
	return z;
}
#endif

#ifndef CMPLX
static __inline double_complex
CMPLX(double x, double y)
{
	double_complex z;
	
	REALPART(z) = x;
	IMAGPART(z) = y;
	//return (z.f);
	return z;
}
#endif

#ifndef CMPLXL
static __inline long_double_complex
CMPLXL(long double x, long double y)
{
	long_double_complex z;

	REALPART(z) = x;
	IMAGPART(z) = y;
	//return (z.f);
	return z;
}
#endif

double_complex __ldexp_cexp(double_complex z, int expt);

#endif /* _COMPLEX_H */

double __ldexp_exp(double x, int expt);
float __ldexp_expf(float x, int expt);

double __kernel_sin(double x, double y, int iy);
double __kernel_cos(double x, double y);

float __kernel_sindf(double x);
float __kernel_cosdf(double x);

double sinpi(double x);
double cospi(double x);

/*
#define	_2sum(a, b) do {	\
	__typeof__(a) __s, __w;	\
				\
	__w = (a) + (b);	\
	__s = __w - (a);	\
	(b) = ((a) - (__w - __s)) + ((b) - __s); \
	(a) = __w;		\
} while (0)

#define	_2sumF(a, b) do {	\
	__typeof__(a) __w;	\
				\
	__w = (a) + (b);	\
	(b) = ((a) - __w) + (b); \
	(a) = __w;		\
} while (0)
*/

#endif