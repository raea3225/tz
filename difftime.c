#ifndef lint
#ifndef NOID
static char	elsieid[] = "%W%";
#endif /* !defined NOID */
#endif /* !defined lint */

/*LINTLIBRARY*/

#include "private.h"

/*
** Algorithm courtesy Paul Eggert (eggert@twinsun.com).
*/

double
difftime(time1, time0)
const time_t	time1;
const time_t	time0;
{
	time_t	delta;
	time_t	hibit;

	if (sizeof(time_t) < sizeof(double))
		return (double) time1 - (double) time0;
#ifdef HAVE_LONG_DOUBLE
	if (sizeof(time_t) < sizeof(long double))
		return (long double) time1 - (long double) time0;
#endif /* defined HAVE_LONG_DOUBLE */
	if (time1 < time0)
		return -difftime(time0, time1);
	/*
	** As much as possible, avoid loss of precision
	** by computing the difference before converting to double.
	*/
	delta = time1 - time0;
	if (delta >= 0)
		return delta;
	/*
	** Repair delta overflow.
	*/
	hibit = 1;
	while ((hibit <<= 1) > 0)
		continue;
	/*
	** The following expression rounds twice, which means
	** the result may not be the closest to the true answer.
	** For example, suppose time_t is 64-bit signed int,
	** double is IEEE 754 double with default rounding,
	** time1 = 9223372036854775807 and time0 = -1536.
	** Then the true difference is 9223372036854777343,
	** which rounds to 9223372036854777856
	** with a total error of 513.
	** But delta overflows to -9223372036854774273,
	** which rounds to -9223372036854774784, and correcting
	** this by subtracting 2 * (double) hibit
	** (i.e. by adding 2**64 = 18446744073709551616)
	** yields 9223372036854776832, which
	** rounds to 9223372036854775808
	** with a total error of 1535 instead.
	** This problem occurs only with very large differences.
	** It's too painful to fix this portably.
	** We are not alone in this problem;
	** many C compilers round twice when converting
	** large unsigned types to small floating types,
	** so if time_t is unsigned the "return delta" above
	** has the same double-rounding problem.
	*/
	return delta - 2 * (double) hibit;
}
