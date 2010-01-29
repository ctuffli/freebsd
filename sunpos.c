/*-
 * Copyright (c) 2009-2010 Edwin Groothuis. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * 
 */

/*
 * This code is created to match the formulas available at:
 * Formula and examples obtained from "How to Calculate alt/az: SAAO" at
 * http://www.saao.ac.za/public-info/sun-moon-stars/sun-index/how-to-calculate-altaz/
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "calendar.h"

#define D2R(m)	((m) / 180 * M_PI)
#define R2D(m)	((m) * 180 / M_PI)

#define	SIN(x)	(sin(D2R(x)))
#define	COS(x)	(cos(D2R(x)))
#define	TAN(x)	(tan(D2R(x)))
#define	ASIN(x)	(R2D(asin(x)))
#define	ATAN(x)	(R2D(atan(x)))

#ifdef NOTDEF
static void
comp(char *s, double v, double c)
{

	printf("%-*s %*g %*g %*g\n", 15, s, 15, v, 15, c, 15, v - c);
}

int expY;
double expZJ = 30.5;
double expUTHM = 8.5;
double expD = 34743.854;
double expT = 0.9512349;
double expL = 324.885;
double expM = 42.029;
double expepsilon = 23.4396;
double explambda = 326.186;
double expalpha = 328.428;
double expDEC = -12.789;
double expeastlongitude = 17.10;
double explatitude = -22.57;
double expHA = -37.673;
double expALT = 49.822;
double expAZ = 67.49;
#endif

static double
fixup(double *d)
{

	if (*d < 0) {
		while (*d < 0)
			*d += 360;
	} else {
		while (*d > 360)
			*d -= 360;
	}

	return (*d);
}

static double ZJtable[] = {
	0, -0.5, 30.5, 58.5, 89.5, 119.5, 150.5, 180.5, 211.5, 242.5, 272.5, 303.5, 333.5 };

static void
sunpos(int inYY, int inMM, int inDD, double UTCOFFSET, int inHOUR, int inMIN,
    double eastlongitude, double latitude, double *DEC, double *ALT, double *AZ)
{
	int Y;
	double ZJ, D, T, L, M, epsilon, lambda, alpha, HA, UTHM;

	/* Not calculated in this code */
	*ALT = 0;
	*AZ = 0;

	ZJ = ZJtable[inMM];
	if (inMM <= 2 && isleap(inYY))
		ZJ -= 1.0;

	UTHM = inHOUR + inMIN / 60.0 + UTCOFFSET;
	Y = inYY - 1900;						/*  1 */
	D = floor(365.25 * Y) + ZJ + inDD + UTHM / 24;			/*  3 */
	T = D / 36525.0;						/*  4 */
	L = 279.697 + 36000.769 * T;					/*  5 */
	fixup(&L);
	M = 358.476 + 35999.050 * T;					/*  6 */
	fixup(&M);
	epsilon = 23.452 - 0.013 * T;					/*  7 */
	fixup(&epsilon);

	lambda = L + (1.919 - 0.005 * T) * SIN(M) + 0.020 * SIN(2 * M);	/*  8 */
	fixup(&lambda);
	alpha = ATAN(TAN(lambda) * COS(epsilon));			/*  9 */

	/* Alpha should be in the same quadrant as lamba */
	if (1) {
		int lssign = sin(D2R(lambda)) < 0 ? -1 : 1;
		int lcsign = cos(D2R(lambda)) < 0 ? -1 : 1;
		while (((sin(D2R(alpha)) < 0) ? -1 : 1) != lssign
		    || ((cos(D2R(alpha)) < 0) ? -1 : 1) != lcsign)
			alpha += 90.0;
	}
	fixup(&alpha);

	*DEC = ASIN(SIN(lambda) * SIN(epsilon));			/* 10 */
	fixup(DEC);
	fixup(&eastlongitude);
	HA = L - alpha + 180 + 15 * UTHM + eastlongitude;		/* 12 */
	fixup(&HA);
	fixup(&latitude);
#ifdef NOTDEF
	printf("%02d/%02d %02d: l:%g d:%g h:%g\n",
	    inMM, inDD, inHOUR, latitude, *DEC, HA);
#endif
	return;

	/*
	 * The following calculations are not used, so to save time
	 * they are not calculated.
	 */
#ifdef NOTDEF
	*ALT = ASIN(SIN(latitude) * SIN(*DEC) +
	    COS(latitude) * COS(*DEC) * COS(HA));			/* 13 */
	fixup(ALT);
	*AZ = ATAN(SIN(HA) /
	    (COS(HA) * SIN(latitude) - TAN(*DEC) * COS(latitude)));	/* 14 */

	if (*ALT > 180)
		*ALT -= 360;
	if (*ALT < -180)
		*ALT += 360;
	printf("a:%g a:%g\n", *ALT, *AZ);
#endif

#ifdef NOTDEF
	printf("Y:\t\t\t     %d\t\t     %d\t\t      %d\n", Y, expY, Y - expY);
	comp("ZJ", ZJ, expZJ);
	comp("UTHM", UTHM, expUTHM);
	comp("D", D, expD);
	comp("T", T, expT);
	comp("L", L, fixup(&expL));
	comp("M", M, fixup(&expM));
	comp("epsilon", epsilon, fixup(&expepsilon));
	comp("lambda", lambda, fixup(&explambda));
	comp("alpha", alpha, fixup(&expalpha));
	comp("DEC", DEC, fixup(&expDEC));
	comp("eastlongitude", eastlongitude, fixup(&expeastlongitude));
	comp("latitude", latitude, fixup(&explatitude));
	comp("HA", HA, fixup(&expHA));
	comp("ALT", ALT, fixup(&expALT));
	comp("AZ", AZ, fixup(&expAZ));
#endif
}


#define	SIGN(a)	(((a) > 180) ? -1 : 1)
#define ANGLE(a, b) (((a) < (b)) ? 1 : -1)
#define HOUR(h) ((h) / 4)
#define MIN(h) (15 * ((h) % 4))
#define	DEBUG1(y, m, d, hh, mm, pdec, dec) \
	printf("%4d-%02d-%02d %02d:%02d:00 - %7.7g -> %7.7g\n", \
	    y, m, d, hh, mm, pdec, dec)
#define	DEBUG2(y, m, d, hh, mm, pdec, dec, pang, ang) \
	printf("%4d-%02d-%02d %02d:%02d:00 - %7.7g -> %7.7g - %d -> %d\n", \
	    y, m, d, hh, mm, pdec, dec, pang, ang)
void
equinoxsolstice(int year, double UTCoffset, int *equinoxdays, int *solsticedays)
{
	double dec, prevdec, alt, az;
	int h, d, prevangle, angle;
	int found = 0;

	int *cumdays;
	cumdays = cumdaytab[isleap(year)];

	/*
	 * Find the first equinox, somewhere in March:
	 * It happens when the returned value "dec" goes from
	 * [350 ... 360> -> [0 ... 10]
	 */
	found = 0;
	prevdec = 350;
	for (d = 18; d < 31; d++) {
		for (h = 0; h < 4 * 24; h++) {
			sunpos(year, 3, d, UTCoffset, HOUR(h), MIN(h),
			    0.0, 0.0, &dec, &alt, &az);
			if (SIGN(prevdec) != SIGN(dec)) {
#ifdef NOTDEF
				DEBUG1(year, 3, d, HOUR(h), MIN(h),
				    prevdec, dec);
#endif
				equinoxdays[0] = cumdays[3] + d;
				found = 1;
				break;
			}
			prevdec = dec;
		}
		if (found)
			break;
	}

	/* Find the second equinox, somewhere in September:
	 * It happens when the returned value "dec" goes from
	 * [10 ... 0] -> <360 ... 350]
	 */
	found = 0;
	prevdec = 10;
	for (d = 18; d < 31; d++) {
		for (h = 0; h < 4 * 24; h++) {
			sunpos(year, 9, d, UTCoffset, HOUR(h), MIN(h),
			    0.0, 0.0, &dec, &alt, &az);
			if (SIGN(prevdec) != SIGN(dec)) {
#ifdef NOTDEF
				DEBUG1(year, 9, d, HOUR(h), MIN(h),
				    prevdec, dec);
#endif
				equinoxdays[1] = cumdays[9] + d;
				found = 1;
				break;
			}
			prevdec = dec;
		}
		if (found)
			break;
	}

	/*
	 * Find the first solstice, somewhere in June:
	 * It happens when the returned value "dec" peaks
	 * [40 ... 45] -> [45 ... 40]
	 */
	found = 0;
	prevdec = 0;
	prevangle = 1;
	for (d = 18; d < 31; d++) {
		for (h = 0; h < 4 * 24; h++) {
			sunpos(year, 6, d, UTCoffset, HOUR(h), MIN(h),
			    0.0, 0.0, &dec, &alt, &az);
			angle = ANGLE(prevdec, dec);
			if (prevangle != angle) {
#ifdef NOTDEF
				DEBUG2(year, 6, d, HOUR(h), MIN(h),
				    prevdec, dec, prevangle, angle);
#endif
				solsticedays[0] = cumdays[6] + d;
				found = 1;
				break;
			}
			prevdec = dec;
			prevangle = angle;
		}
		if (found)
			break;
	}

	/*
	 * Find the second solstice, somewhere in December:
	 * It happens when the returned value "dec" peaks
	 * [315 ... 310] -> [310 ... 315]
	 */
	found = 0;
	prevdec = 360;
	prevangle = -1;
	for (d = 18; d < 31; d++) {
		for (h = 0; h < 4 * 24; h++) {
			sunpos(year, 12, d, UTCoffset, HOUR(h), MIN(h),
			    0.0, 0.0, &dec, &alt, &az);
			angle = ANGLE(prevdec, dec);
			if (prevangle != angle) {
#ifdef NOTDEF
				DEBUG2(year, 12, d, HOUR(h), MIN(h),
				    prevdec, dec, prevangle, angle);
#endif
				solsticedays[1] = cumdays[12] + d;
				found = 1;
				break;
			}
			prevdec = dec;
			prevangle = angle;
		}
		if (found)
			break;
	}

	return;
}

#ifdef NOTDEF
int
main(int argc, char **argv)
{
/*
	year      Mar        June       Sept       Dec
	     day   time  day   time  day time  day time
	2004  20   06:49  21   00:57  22  16:30 21  12:42
	2005  20   12:33  21   06:46  22  22:23 21  18:35
	2006  20   18:26  21   12:26  23  04:03 22  00:22
	2007  21   00:07  21   18:06  23  09:51 22  06:08
	2008  20   05:48  20   23:59  22  15:44 21  12:04
	2009  20   11:44  21   05:45  22  21:18 21  17:47
	2010  20   17:32  21   11:28  23  03:09 21  23:38
	2011  20   23:21  21   17:16  23  09:04 22  05:30
	2012  20   05:14  20   23:09  22  14:49 21  11:11
	2013  20   11:02  21   05:04  22  20:44 21  17:11
	2014  20   16:57  21   10:51  23  02:29 21  23:03
	2015  20   22:45  21   16:38  23  08:20 22  04:48
	2016  20   04:30  20   22:34  22  14:21 21  10:44
	2017  20   10:28  21   04:24  22  20:02 21  16:28
*/

	int eq[2], sol[2];
	equinoxsolstice(strtol(argv[1], NULL, 10), 0.0, eq, sol);
	printf("%d - %d - %d - %d\n", eq[0], sol[0], eq[1], sol[1]);
	return(0);
}
#endif
