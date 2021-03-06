/* dlaed6.f -- translated by f2c (version 20061008).
   You must link the resulting object file with libf2c:
	on Microsoft Windows system, link with libf2c.lib;
	on Linux or Unix systems, link with .../path/to/libf2c.a -lm
	or, if you install libf2c.a in a standard place, with -lf2c -lm
	-- in that order, at the end of the command line, as in
		cc *.o -lf2c -lm
	Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

		http://www.netlib.org/f2c/libf2c.zip
*/

#include "pnl/pnl_f2c.h"

 int dlaed6_(int *kniter, int *orgati, double *
	rho, double *d__, double *z__, double *finit, double *
	tau, int *info)
{
    /* System generated locals */
    int i__1;
    double d__1, d__2, d__3, d__4;

    /* Builtin functions */
    double sqrt(double), log(double), pow_di(double *, int *);

    /* Local variables */
    double a, b, c__, f;
    int i__;
    double fc, df, ddf, lbd, eta, ubd, eps, base;
    int iter;
    double temp, temp1, temp2, temp3, temp4;
    int scale;
    int niter;
    double small1, small2, sminv1, sminv2;
    extern double dlamch_(char *);
    double dscale[3], sclfac, zscale[3], erretm, sclinv;


/*  -- LAPACK routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     February 2007 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAED6 computes the positive or negative root (closest to the origin) */
/*  of */
/*                   z(1)        z(2)        z(3) */
/*  f(x) =   rho + --------- + ---------- + --------- */
/*                  d(1)-x      d(2)-x      d(3)-x */

/*  It is assumed that */

/*        if ORGATI = .true. the root is between d(2) and d(3); */
/*        otherwise it is between d(1) and d(2) */

/*  This routine will be called by DLAED4 when necessary. In most cases, */
/*  the root sought is the smallest in magnitude, though it might not be */
/*  in some extremely rare situations. */

/*  Arguments */
/*  ========= */

/*  KNITER       (input) INTEGER */
/*               Refer to DLAED4 for its significance. */

/*  ORGATI       (input) LOGICAL */
/*               If ORGATI is true, the needed root is between d(2) and */
/*               d(3); otherwise it is between d(1) and d(2).  See */
/*               DLAED4 for further details. */

/*  RHO          (input) DOUBLE PRECISION */
/*               Refer to the equation f(x) above. */

/*  D            (input) DOUBLE PRECISION array, dimension (3) */
/*               D satisfies d(1) < d(2) < d(3). */

/*  Z            (input) DOUBLE PRECISION array, dimension (3) */
/*               Each of the elements in z must be positive. */

/*  FINIT        (input) DOUBLE PRECISION */
/*               The value of f at 0. It is more accurate than the one */
/*               evaluated inside this routine (if someone wants to do */
/*               so). */

/*  TAU          (output) DOUBLE PRECISION */
/*               The root of the equation f(x). */

/*  INFO         (output) INTEGER */
/*               = 0: successful exit */
/*               > 0: if INFO = 1, failure to converge */

/*  Further Details */
/*  =============== */

/*  30/06/99: Based on contributions by */
/*     Ren-Cang Li, Computer Science Division, University of California */
/*     at Berkeley, USA */

/*  10/02/03: This version has a few statements commented out for thread */
/*  safety (machine parameters are computed on each entry). SJH. */

/*  05/10/06: Modified from a new version of Ren-Cang Li, use */
/*     Gragg-Thornton-Warner cubic convergent scheme for better stability. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Local Arrays .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --z__;
    --d__;

    /* Function Body */
    *info = 0;

    if (*orgati) {
	lbd = d__[2];
	ubd = d__[3];
    } else {
	lbd = d__[1];
	ubd = d__[2];
    }
    if (*finit < 0.) {
	lbd = 0.;
    } else {
	ubd = 0.;
    }

    niter = 1;
    *tau = 0.;
    if (*kniter == 2) {
	if (*orgati) {
	    temp = (d__[3] - d__[2]) / 2.;
	    c__ = *rho + z__[1] / (d__[1] - d__[2] - temp);
	    a = c__ * (d__[2] + d__[3]) + z__[2] + z__[3];
	    b = c__ * d__[2] * d__[3] + z__[2] * d__[3] + z__[3] * d__[2];
	} else {
	    temp = (d__[1] - d__[2]) / 2.;
	    c__ = *rho + z__[3] / (d__[3] - d__[2] - temp);
	    a = c__ * (d__[1] + d__[2]) + z__[1] + z__[2];
	    b = c__ * d__[1] * d__[2] + z__[1] * d__[2] + z__[2] * d__[1];
	}
/* Computing MAX */
	d__1 = ABS(a), d__2 = ABS(b), d__1 = MAX(d__1,d__2), d__2 = ABS(c__);
	temp = MAX(d__1,d__2);
	a /= temp;
	b /= temp;
	c__ /= temp;
	if (c__ == 0.) {
	    *tau = b / a;
	} else if (a <= 0.) {
	    *tau = (a - sqrt((d__1 = a * a - b * 4. * c__, ABS(d__1)))) / (
		    c__ * 2.);
	} else {
	    *tau = b * 2. / (a + sqrt((d__1 = a * a - b * 4. * c__, ABS(d__1))
		    ));
	}
	if (*tau < lbd || *tau > ubd) {
	    *tau = (lbd + ubd) / 2.;
	}
	if (d__[1] == *tau || d__[2] == *tau || d__[3] == *tau) {
	    *tau = 0.;
	} else {
	    temp = *finit + *tau * z__[1] / (d__[1] * (d__[1] - *tau)) + *tau 
		    * z__[2] / (d__[2] * (d__[2] - *tau)) + *tau * z__[3] / (
		    d__[3] * (d__[3] - *tau));
	    if (temp <= 0.) {
		lbd = *tau;
	    } else {
		ubd = *tau;
	    }
	    if (ABS(*finit) <= ABS(temp)) {
		*tau = 0.;
	    }
	}
    }

/*     get machine parameters for possible scaling to avoid overflow */

/*     modified by Sven: parameters SMALL1, SMINV1, SMALL2, */
/*     SMINV2, EPS are not SAVEd anymore between one call to the */
/*     others but recomputed at each call */

    eps = dlamch_("Epsilon");
    base = dlamch_("Base");
    i__1 = (int) (log(dlamch_("SafMin")) / log(base) / 3.);
    small1 = pow_di(&base, &i__1);
    sminv1 = 1. / small1;
    small2 = small1 * small1;
    sminv2 = sminv1 * sminv1;

/*     Determine if scaling of inputs necessary to avoid overflow */
/*     when computing 1/TEMP**3 */

    if (*orgati) {
/* Computing MIN */
	d__3 = (d__1 = d__[2] - *tau, ABS(d__1)), d__4 = (d__2 = d__[3] - *
		tau, ABS(d__2));
	temp = MIN(d__3,d__4);
    } else {
/* Computing MIN */
	d__3 = (d__1 = d__[1] - *tau, ABS(d__1)), d__4 = (d__2 = d__[2] - *
		tau, ABS(d__2));
	temp = MIN(d__3,d__4);
    }
    scale = FALSE;
    if (temp <= small1) {
	scale = TRUE;
	if (temp <= small2) {

/*        Scale up by power of radix nearest 1/SAFMIN**(2/3) */

	    sclfac = sminv2;
	    sclinv = small2;
	} else {

/*        Scale up by power of radix nearest 1/SAFMIN**(1/3) */

	    sclfac = sminv1;
	    sclinv = small1;
	}

/*        Scaling up safe because D, Z, TAU scaled elsewhere to be O(1) */

	for (i__ = 1; i__ <= 3; ++i__) {
	    dscale[i__ - 1] = d__[i__] * sclfac;
	    zscale[i__ - 1] = z__[i__] * sclfac;
/* L10: */
	}
	*tau *= sclfac;
	lbd *= sclfac;
	ubd *= sclfac;
    } else {

/*        Copy D and Z to DSCALE and ZSCALE */

	for (i__ = 1; i__ <= 3; ++i__) {
	    dscale[i__ - 1] = d__[i__];
	    zscale[i__ - 1] = z__[i__];
/* L20: */
	}
    }

    fc = 0.;
    df = 0.;
    ddf = 0.;
    for (i__ = 1; i__ <= 3; ++i__) {
	temp = 1. / (dscale[i__ - 1] - *tau);
	temp1 = zscale[i__ - 1] * temp;
	temp2 = temp1 * temp;
	temp3 = temp2 * temp;
	fc += temp1 / dscale[i__ - 1];
	df += temp2;
	ddf += temp3;
/* L30: */
    }
    f = *finit + *tau * fc;

    if (ABS(f) <= 0.) {
	goto L60;
    }
    if (f <= 0.) {
	lbd = *tau;
    } else {
	ubd = *tau;
    }

/*        Iteration begins -- Use Gragg-Thornton-Warner cubic convergent */
/*                            scheme */

/*     It is not hard to see that */

/*           1) Iterations will go up monotonically */
/*              if FINIT < 0; */

/*           2) Iterations will go down monotonically */
/*              if FINIT > 0. */

    iter = niter + 1;

    for (niter = iter; niter <= 40; ++niter) {

	if (*orgati) {
	    temp1 = dscale[1] - *tau;
	    temp2 = dscale[2] - *tau;
	} else {
	    temp1 = dscale[0] - *tau;
	    temp2 = dscale[1] - *tau;
	}
	a = (temp1 + temp2) * f - temp1 * temp2 * df;
	b = temp1 * temp2 * f;
	c__ = f - (temp1 + temp2) * df + temp1 * temp2 * ddf;
/* Computing MAX */
	d__1 = ABS(a), d__2 = ABS(b), d__1 = MAX(d__1,d__2), d__2 = ABS(c__);
	temp = MAX(d__1,d__2);
	a /= temp;
	b /= temp;
	c__ /= temp;
	if (c__ == 0.) {
	    eta = b / a;
	} else if (a <= 0.) {
	    eta = (a - sqrt((d__1 = a * a - b * 4. * c__, ABS(d__1)))) / (c__ 
		    * 2.);
	} else {
	    eta = b * 2. / (a + sqrt((d__1 = a * a - b * 4. * c__, ABS(d__1)))
		    );
	}
	if (f * eta >= 0.) {
	    eta = -f / df;
	}

	*tau += eta;
	if (*tau < lbd || *tau > ubd) {
	    *tau = (lbd + ubd) / 2.;
	}

	fc = 0.;
	erretm = 0.;
	df = 0.;
	ddf = 0.;
	for (i__ = 1; i__ <= 3; ++i__) {
	    temp = 1. / (dscale[i__ - 1] - *tau);
	    temp1 = zscale[i__ - 1] * temp;
	    temp2 = temp1 * temp;
	    temp3 = temp2 * temp;
	    temp4 = temp1 / dscale[i__ - 1];
	    fc += temp4;
	    erretm += ABS(temp4);
	    df += temp2;
	    ddf += temp3;
/* L40: */
	}
	f = *finit + *tau * fc;
	erretm = (ABS(*finit) + ABS(*tau) * erretm) * 8. + ABS(*tau) * df;
	if (ABS(f) <= eps * erretm) {
	    goto L60;
	}
	if (f <= 0.) {
	    lbd = *tau;
	} else {
	    ubd = *tau;
	}
/* L50: */
    }
    *info = 1;
L60:

/*     Undo scaling */

    if (scale) {
	*tau *= sclinv;
    }
    return 0;

/*     End of DLAED6 */

} /* dlaed6_ */
