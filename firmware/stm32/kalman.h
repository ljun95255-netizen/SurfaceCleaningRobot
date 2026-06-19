#ifndef  _KEM_H
#define  _KEM_H


double
kalman(
	double			t,		/* Time */
	double			q,		/* Pitching gyro */
	double			ax,		/* X acceleration */
	double			ay		/* Y acceleration */
);

#endif
