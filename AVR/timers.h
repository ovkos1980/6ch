#ifndef _TIMERS_H
#define _TIMERS_H

/* common definitions */
#define NUMTIMERS      10U    /* Number of required timers */
#define TIMERTIC       10U    /* Timer period 17 ms        */
#define BASETIMER      0U     /* Free running system timer */

#define ONE_TIMERTIC   (1/50*1000UL/TIMERTIC)
#define TWO_TIMERTIC   (2/50*1000UL/TIMERTIC)
#define THREE_TIMERTIC (3/50*1000UL/TIMERTIC)
#define FOUR_TIMERTIC  (4/50*1000UL/TIMERTIC)

#define QUARTER_SEC    (1/4*1000UL/TIMERTIC)
#define HALF_SEC       (1/2*1000UL/TIMERTIC)
#define ONE_SEC        (1*1000UL/TIMERTIC)
#define TWO_SEC        (2*1000UL/TIMERTIC)
#define THREE_SEC      (3*1000UL/TIMERTIC)
#define FOUR_SEC       (4*1000UL/TIMERTIC)
#define FIVE_SEC       (5*1000UL/TIMERTIC)
#define ONE_MIN        (1*60*1000UL/TIMERTIC)
#define TWO_MIN        (2*60*1000UL/TIMERTIC)
#define THREE_MIN      (3*60*1000UL/TIMERTIC)
#define FOUR_MIN       (4*60*1000UL/TIMERTIC)
#define FIVE_MIN       (5*60*1000UL/TIMERTIC)

struct timers
 {
  uint16_t TimerField;
  uint8_t Free;
 };


/* API for Timers */
void     init_timerpool(void);
uint8_t  get_timer(void);
void     free_timer(uint8_t nbr);
void     init_timer(uint8_t nbr, uint32_t tout);
uint32_t check_timer(uint8_t nbr);
//void irqhandler_timer(void);

#endif //_TIMERS_H
