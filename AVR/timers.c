#include "defs.h"
#include "timers.h"

volatile struct timers TimerPool[NUMTIMERS];

//---------------------------------------------------------------------
// назъбкемхе: void init_timerpool (void)
// нохяюмхе: 
// оюпюлерпш:
// бшунд:
//---------------------------------------------------------------------
void init_timerpool (void)
 {
   uint8_t i;
   
   TCCR2B |= ((1<<CS22)|(1<<CS21)|(1<<CS20));  // clk/1024
   OCR2A = (TIMERTIC*F_CLK)/(1024UL*1000UL);   // value for compare 
   TIMSK2 |= (1<<OCIE2A);                      // compare match interrupt enable 
    
   for( i=0; i < NUMTIMERS; i++)
    {
      TimerPool[i].TimerField = 0;
      TimerPool[i].Free = 1;
    }

 }


//---------------------------------------------------------------------
// назъбкемхе: UINT8 get_timer (void)
// нохяюмхе: 
// оюпюлерпш:
// бшунд:
//---------------------------------------------------------------------
uint8_t get_timer(void)
 {
   uint8_t i;
   uint8_t FirstMatch;

   for( i=0; i < NUMTIMERS; i++)
    {
     if( TimerPool[i].Free )             /* We found a free timer! */
      {
	TimerPool[i].Free = 0;           /* Mark is reserved */
	FirstMatch = i;
	return FirstMatch;		 /* Return Handle */
      }
    }
   
   /* Error Check	*/
   //DEBUGOUT("No Timers, Resetting..\n\r");
   //RESET_SYSTEM();
   return(0);
 }



//---------------------------------------------------------------------
// назъбкемхе: void free_timer (UINT8 nbr)
// нохяюмхе: 
// оюпюлерпш:
// бшунд:
//---------------------------------------------------------------------
void free_timer (uint8_t nbr)
 {
   if( nbr > (NUMTIMERS-1) )   /* Make a simple check */
    return;
   TimerPool[nbr].Free = 1;
 }



//---------------------------------------------------------------------
// назъбкемхе: void init_timer ( UINT8 nbr, UINT32 tout )
// нохяюмхе: 
// оюпюлерпш:
// бшунд:
//---------------------------------------------------------------------
void init_timer ( uint8_t nbr, uint32_t tout )
 {
   uint32_t val;

   if( nbr > (NUMTIMERS-1) )  return;       /* Make a simple check */
   if( TimerPool[nbr].Free )  return;
   
   /* All OK */
   val = tout;
   //DISABLE_TIMER_IRQ();
   /* Normalize seconds to timer tics	*/
   TimerPool[nbr].TimerField = val;
   //ENABLE_TIMER_IRQ();
}



//---------------------------------------------------------------------
// назъбкемхе: UINT32 check_timer (UINT8 nbr)
// нохяюмхе: 
// оюпюлерпш:
// бшунд:
//---------------------------------------------------------------------
uint32_t check_timer (uint8_t nbr)
 {
   /* This should be quite fast function! */
   /* Interrupts are not disabled when fetching the */
   /* value, therefore returned value possible has */
   /* an error component +/- TIMERTIC. */
   /* Just live with it! */
   return TimerPool[nbr].TimerField;
 }



//---------------------------------------------------------------------
// назъбкемхе: void irqhandler_timer (void)
// нохяюмхе: Interrupt request every 20 ms intrval
// оюпюлерпш:
// бшунд:
//---------------------------------------------------------------------
#pragma vector = TIMER2_COMPA_vect
__interrupt void irqhandler_timer (void)
 {
   uint16_t i;

   TCNT2 = 0x00;   /* clear timer */
   
   for( i=0; i<NUMTIMERS; i++ )
    {
      if(( !TimerPool[i].Free ) && ( TimerPool[i].TimerField ))
      TimerPool[i].TimerField --;
    }
 }
