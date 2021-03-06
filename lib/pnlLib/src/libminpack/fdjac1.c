/* Minpack Copyright Notice (1999) University of Chicago.  All rights reserved
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the
 * following conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 * copyright notice, this list of conditions and the following
 * disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials
 * provided with the distribution.
 *
 * 3. The end-user documentation included with the
 * redistribution, if any, must include the following
 * acknowledgment:
 *
 *    "This product includes software developed by the
 *    University of Chicago, as Operator of Argonne National
 *    Laboratory.
 *
 * Alternately, this acknowledgment may appear in the software
 * itself, if and wherever such third-party acknowledgments
 * normally appear.
 *
 * 4. WARRANTY DISCLAIMER. THE SOFTWARE IS SUPPLIED "AS IS"
 * WITHOUT WARRANTY OF ANY KIND. THE COPYRIGHT HOLDER, THE
 * UNITED STATES, THE UNITED STATES DEPARTMENT OF ENERGY, AND
 * THEIR EMPLOYEES: (1) DISCLAIM ANY WARRANTIES, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO ANY IMPLIED WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE
 * OR NON-INFRINGEMENT, (2) DO NOT ASSUME ANY LEGAL LIABILITY
 * OR RESPONSIBILITY FOR THE ACCURACY, COMPLETENESS, OR
 * USEFULNESS OF THE SOFTWARE, (3) DO NOT REPRESENT THAT USE OF
 * THE SOFTWARE WOULD NOT INFRINGE PRIVATELY OWNED RIGHTS, (4)
 * DO NOT WARRANT THAT THE SOFTWARE WILL FUNCTION
 * UNINTERRUPTED, THAT IT IS ERROR-FREE OR THAT ANY ERRORS WILL
 * BE CORRECTED.
 *
 * 5. LIMITATION OF LIABILITY. IN NO EVENT WILL THE COPYRIGHT
 * HOLDER, THE UNITED STATES, THE UNITED STATES DEPARTMENT OF
 * ENERGY, OR THEIR EMPLOYEES: BE LIABLE FOR ANY INDIRECT,
 * INCIDENTAL, CONSEQUENTIAL, SPECIAL OR PUNITIVE DAMAGES OF
 * ANY KIND OR NATURE, INCLUDING BUT NOT LIMITED TO LOSS OF
 * PROFITS OR LOSS OF DATA, FOR ANY REASON WHATSOEVER, WHETHER
 * SUCH LIABILITY IS ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE OR STRICT LIABILITY), OR OTHERWISE,
 * EVEN IF ANY OF SAID PARTIES HAS BEEN WARNED OF THE
 * POSSIBILITY OF SUCH LOSS OR DAMAGES.
 */

#include <math.h>
#include "cminpack.h"

/* subroutine fdjac1
 *
 * this subroutine computes a forward-difference approximation
 * to the n by n jacobian matrix associated with a specified
 * problem of n functions in n variables. if the jacobian has
 * a banded form, then function evaluations are saved by only
 * approximating the nonzero terms.
 *
 * the subroutine statement is
 *
 *   subroutine pnl_minpack_fdjac1(fcn,n,x,fvec,fjac,ldfjac,iflag,ml,mu,epsfcn,
 *                     wa1,wa2)
 *
 * where
 *
 *   fcn is the name of the user-supplied subroutine which
 *     calculates the functions. fcn must be declared
 *     in an external statement in the user calling
 *     program, and should be written as follows.
 *
 *     subroutine fcn(n,x,fvec,iflag)
 *     integer n,iflag
 *     double precision x(n),fvec(n)
 *     ----------
 *     calculate the functions at x and
 *     return this vector in fvec.
 *     ----------
 *     return
 *     end
 *
 *     the value of iflag should not be changed by fcn unless
 *     the user wants to terminate execution of fdjac1.
 *     in this case set iflag to a negative integer.
 *
 *   n is a positive integer input variable set to the number
 *     of functions and variables.
 *
 *   x is an input array of length n.
 *
 *   fvec is an input array of length n which must contain the
 *     functions evaluated at x.
 *
 *   fjac is an output n by n array which contains the
 *     approximation to the jacobian matrix evaluated at x.
 *
 *   ldfjac is a positive integer input variable not less than n
 *     which specifies the leading dimension of the array fjac.
 *
 *   iflag is an integer variable which can be used to terminate
 *     the execution of fdjac1. see description of fcn.
 *
 *   ml is a nonnegative integer input variable which specifies
 *     the number of subdiagonals within the band of the
 *     jacobian matrix. if the jacobian is not banded, set
 *     ml to at least n - 1.
 *
 *   epsfcn is an input variable used in determining a suitable
 *     step length for the forward-difference approximation. this
 *     approximation assumes that the relative errors in the
 *     functions are of the order of epsfcn. if epsfcn is less
 *     than the machine precision, it is assumed that the relative
 *     errors in the functions are of the order of the machine
 *     precision.
 *
 *   mu is a nonnegative integer input variable which specifies
 *     the number of superdiagonals within the band of the
 *     jacobian matrix. if the jacobian is not banded, set
 *     mu to at least n - 1.
 *
 *   wa1 and wa2 are work arrays of length n. if ml + mu + 1 is at
 *     least n, then the jacobian is considered dense, and wa2 is
 *     not referenced.
 *
 * subprograms called
 *
 *   minpack-supplied ... dpmpar
 *
 *   fortran-supplied ... dabs,dmax1,dsqrt
 *
 * argonne national laboratory. minpack project. march 1980.
 * burton s. garbow, kenneth e. hillstrom, jorge j. more
 */


int pnl_minpack_fdjac1(minpack_func_nn fcn, void *p, int n, double *x, const double *
                       fvec, double *fjac, int ldfjac, int ml,
                       int mu, double epsfcn, double *wa1, double *wa2)
{
  /* System generated locals */
  int fjac_dim1, fjac_offset, i__1, i__2, i__3, i__4;

  /* Local variables */
  double h__;
  int i__, j, k;
  double eps, temp;
  int msum;
  double epsmch;
  int iflag = 0;

  /* Parameter adjustments */
  --wa2;
  --wa1;
  --fvec;
  --x;
  fjac_dim1 = ldfjac;
  fjac_offset = 1 + fjac_dim1 * 1;
  fjac -= fjac_offset;

  /* Function Body */

  /*     epsmch is the machine precision. */

  epsmch = pnl_minpack_dpmpar(1);

  eps = sqrt((MAX(epsfcn, epsmch)));
  msum = ml + mu + 1;
  if (msum < n)
    {
      goto L40;
    }

  /*        computation of dense approximate jacobian. */

  i__1 = n;
  for (j = 1; j <= i__1; ++j)
    {
      temp = x[j];
      h__ = eps * fabs(temp);
      if (h__ == 0.)
        {
          h__ = eps;
        }
      x[j] = temp + h__;
      iflag = (*fcn)(p, n, &x[1], &wa1[1], 1);
      if (iflag < 0)
        {
          goto L30;
        }
      x[j] = temp;
      i__2 = n;
      for (i__ = 1; i__ <= i__2; ++i__)
        {
          fjac[i__ + j * fjac_dim1] = (wa1[i__] - fvec[i__]) / h__;
          /* L10: */
        }
      /* L20: */
    }
L30:
  /* goto L110; */
  return iflag;
L40:

  /*        computation of banded approximate jacobian. */

  i__1 = msum;
  for (k = 1; k <= i__1; ++k)
    {
      i__2 = n;
      i__3 = msum;
      for (j = k; i__3 < 0 ? j >= i__2 : j <= i__2; j += i__3)
        {
          wa2[j] = x[j];
          h__ = eps * fabs(wa2[j]);
          if (h__ == 0.)
            {
              h__ = eps;
            }
          x[j] = wa2[j] + h__;
          /* L60: */
        }
      iflag = (*fcn)(p, n, &x[1], &wa1[1], 1);
      if (iflag < 0)
        {
          /* goto L100; */
          return iflag;
        }
      i__3 = n;
      i__2 = msum;
      for (j = k; i__2 < 0 ? j >= i__3 : j <= i__3; j += i__2)
        {
          x[j] = wa2[j];
          h__ = eps * fabs(wa2[j]);
          if (h__ == 0.)
            {
              h__ = eps;
            }
          i__4 = n;
          for (i__ = 1; i__ <= i__4; ++i__)
            {
              fjac[i__ + j * fjac_dim1] = 0.;
              if (i__ >= j - mu && i__ <= j + ml)
                {
                  fjac[i__ + j * fjac_dim1] = (wa1[i__] - fvec[i__]) / h__;
                }
              /* L70: */
            }
          /* L80: */
        }
      /* L90: */
    }
  /* L100: */
  /* L110: */
  return iflag;

  /*     last card of subroutine fdjac1. */

} /* fdjac1_ */

