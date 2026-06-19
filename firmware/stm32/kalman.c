/*******************************************************************************
* File Name          : kalman.c
* Author             : www.anybot.cn-------redwolf
* Version            : V1.0
* Date               : 20/07/2009
* Description        : 
********************************************************************************/
#include <stdlib.h>
#include <math.h>


#define		pi		  3.1415926
const double	dt		= 1.0 / 100.0;		/* 50 Hz sensor update */
const double	Q		= 0.0002;						/* Noise weighting matrix */
static double	theta	= -4*pi/180;			/* Our initial state estimate */
static double	R		= 1;								/* Measurement error weight */
static double	P		= 1;								/* Covariance matrix */


double
kalman(
	double			t,		/* Time */
	double			q,		/* Pitching gyro */
	double			ax,		/* X acceleration */
	double			ay		/* Y acceleration */
)
{
	double			Pdot;		/* Derivative of P */
	double			E;		/* ? */
	double			K;		/* ? */

	double			theta_m;	/* Our state measurement */

						/* A = 0 */
	Pdot = Q;				/* Pdot = A*P + P*A' + Q */
	P += Pdot * dt;

	/* Update our state estimate from the rate gyro */
	theta += q * dt;

	/* Compute our measured state from the accelerometers */
	theta_m = atan2( ax, ay );

	E = P + R;				/* E = CPC' + R */
	K = P / E;				/* K = PC'inv(E) */

	/* Update the state */
	theta += K * ( theta_m - theta );

	/* Covariance update */
	P *= ( 1 - K );

	return theta;
}

