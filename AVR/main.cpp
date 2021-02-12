#include "defs.h"
#include "timers.h"
#include "uart.h"
#include "core.h"
#include "shell.h"
#include "pcf8583.h"



void init()
 {
   init_UART(115200);
   init_timerpool();
   init_shell();
   init_core();
   init_rtc();
   
   _SEI();
 }


int main()
{
  init();
  
  uint8_t startup_timer = get_timer();
  init_timer(startup_timer, THREE_SEC);
  
  //while(!hmi_en) poll_pkt();
  while(check_timer(startup_timer));
  
  switch_mode();
  //on_state();
  refresh_buttons();
  
  
  while(1)
   {
    switch(mode)
     {
      case MODE_MANUAL: {
                          poll_pkt();
                          if(core_en)
                           { 
                            poll_core();
                            poll_hmi();
                           }
                        } break;
      case MODE_AUTO: {
                        poll_pkt();
                        if(core_en)
                         { 
                          poll_core(); 
                          poll_hmi();
                          poll_prog();
                         } 
                      } break;
      default: break;
       
    
    
    
     }
   }  
  return 0;
}
