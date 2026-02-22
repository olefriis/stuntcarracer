
#ifndef	_OPPONENT_BEHAVIOUR
#define	_OPPONENT_BEHAVIOUR

/*	========= */
/*	Constants */
/*	========= */
#define	NO_OPPONENT	(-1)

/*	===================== */
/*	Structure definitions */
/*	===================== */

/*	============================== */
/*	External function declarations */
/*	============================== */
extern void OpponentBehaviour (long *x,
							   long *y,
							   long *z,
							   float *x_angle,
							   float *y_angle,
							   float *z_angle,
							   bool bOpponentPaused);

extern void CarToCarCollision( void );

extern long CalculateIfWinning( long start_finish_piece );

extern long CalculateOpponentsDistance (void);

// Two-player support
extern void SetOpponentNetworkState(long roadSection, long distIntoSection, long roadXPos,
                                    long zSpeed, long playerFL, long playerFR, long playerR);
extern void GetOpponentState(long *roadSection, long *distIntoSection, long *roadXPos,
                             long *zSpeed, long *wheelRL, long *wheelRR, long *wheelF);
extern void OpponentBehaviourTwoPlayer(long *x, long *y, long *z,
                                       float *x_angle, float *y_angle, float *z_angle);

#endif	/* _OPPONENT_BEHAVIOUR */
