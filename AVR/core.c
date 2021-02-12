#include "defs.h"
#include "core.h"
#include "shell.h"
#include "uart.h"
#include "timers.h"
#include "pcf8583.h"
#include "hmi.h"

uint8_t mode;
uint8_t hmi_en = 0;
uint8_t core_en = 0;
uint8_t download_ch = 0;  // for download program from HMI purpose
channel_t ch[6] = {channel_t(0), channel_t(1), channel_t(2), channel_t(3), channel_t(4), channel_t(5)};


struct EEPROM_data
 {
   uint8_t EEPROM_attrib;
   uint8_t EEPROM_hyst;
 };

struct EEPROM_prog
 {
   uint8_t inum;        // interval number
   uint8_t def_state;   // default state
   uint16_t ch_prog[MAX_INTERVAL_NUMBER*2];
 };

// EEPROM variables
__eeprom volatile EEPROM_data EEPROM_channel_data[6] @ EEPROM_ATTRIB_ADDRESS = { // first time init data
  {(0<<ATTRIB_NUMBER_OFFSET)|(0<<ATTRIB_MODE_OFFSET)|(0<<ATTRIB_STATUS_OFFSET)|(0<<ATTRIB_STATE_OFFSET),1},
  {(1<<ATTRIB_NUMBER_OFFSET)|(0<<ATTRIB_MODE_OFFSET)|(0<<ATTRIB_STATUS_OFFSET)|(0<<ATTRIB_STATE_OFFSET),1},
  {(2<<ATTRIB_NUMBER_OFFSET)|(0<<ATTRIB_MODE_OFFSET)|(0<<ATTRIB_STATUS_OFFSET)|(0<<ATTRIB_STATE_OFFSET),1},
  {(3<<ATTRIB_NUMBER_OFFSET)|(0<<ATTRIB_MODE_OFFSET)|(0<<ATTRIB_STATUS_OFFSET)|(0<<ATTRIB_STATE_OFFSET),1},
  {(4<<ATTRIB_NUMBER_OFFSET)|(0<<ATTRIB_MODE_OFFSET)|(0<<ATTRIB_STATUS_OFFSET)|(0<<ATTRIB_STATE_OFFSET),1},
  {(5<<ATTRIB_NUMBER_OFFSET)|(0<<ATTRIB_MODE_OFFSET)|(0<<ATTRIB_STATUS_OFFSET)|(0<<ATTRIB_STATE_OFFSET),1}};

__eeprom volatile uint8_t EEPROM_mode @ EEPROM_MODE_ADDRESS = 0;  // relay mode for store in eeprom 0 - manual, 1 - auto

__eeprom volatile EEPROM_prog EEPROM_ch_prog[6] @ EEPROM_CH_PROG_ADDRESS = {
  {2,0,{1,10,15,20,0,0,0,0}},
  {4,0,{2,11,16,21,345,378,1206,1333}},
  {0,0,{0,0,0,0,0,0,0,0}},
  {0,0,{0,0,0,0,0,0,0,0}},
  {0,0,{0,0,0,0,0,0,0,0}},
  {0,0,{0,0,0,0,0,0,0,0}}};

EEPROM_prog prog[6];

// methods description
//---------------------------------------------------------------------
// Œ¡⁄ﬂ¬À≈Õ»≈: channel_t(uint8_t n)
// Œœ»—¿Õ»≈: constructor
// œ¿–¿Ã≈“–€:
// ¬€’Œƒ: 
//---------------------------------------------------------------------
channel_t::channel_t(uint8_t n)
 {
   attrib = EEPROM_channel_data[n].EEPROM_attrib;
   attrib &= ~(ATTRIB_STATE_MASK);    // all channel after power on is OFF
   hyst = EEPROM_channel_data[n].EEPROM_hyst;
   
   switch((attrib & ATTRIB_STATUS_MASK)>>ATTRIB_STATUS_OFFSET)
    {
     case 0: flag = FLAG_POS; break;
     case 1: flag = FLAG_POS; break;
     case 2: flag = FLAG_CLEAR; break;
     default: break;
    }
  
   //uint8_t a = EEPROM_ch_prog[n].inum;
 }


void channel_t::time_service_init(void)
 {
   timer = get_timer();
   init_timer(timer, 0);
 }





//---------------------------------------------------------------------
// Œ¡⁄ﬂ¬À≈Õ»≈: void channel_t::poll(void)
// Œœ»—¿Õ»≈: 
// œ¿–¿Ã≈“–€:
// ¬€’Œƒ: 
//---------------------------------------------------------------------
void channel_t::poll(void)
 {
     
  if(!check_timer(timer))   // if channel timer == 0
  { 
   switch(flag)
    {
     case FLAG_ON: {  
                    flag = FLAG_CLEAR;
                      switch((attrib & ATTRIB_NUMBER_MASK)>>ATTRIB_NUMBER_OFFSET)
                       {
                        case 0: {
                                 LHALF_CH_PORT |= (1<<CH1);
                                 print("vis p0,1");
                                } break;
                        case 1: {
                                 LHALF_CH_PORT |= (1<<CH2);
                                 print("vis p1,1");
                                } break;
                        case 2: {
                                 LHALF_CH_PORT |= (1<<CH3);
                                 print("vis p2,1");
                                } break;
                        case 3: {
                                 HHALF_CH_PORT |= (1<<CH4);
                                 print("vis p3,1");
                                } break;
                        case 4: {
                                 HHALF_CH_PORT |= (1<<CH5);
                                 print("vis p4,1");
                                } break;
                        case 5: {
                                 HHALF_CH_PORT |= (1<<CH6);
                                 print("vis p5,1");
                                } break;
                        default: break;
                      }
                     ATTRIB_STATE_SET(attrib);
                     EEPROM_channel_data[attrib>>ATTRIB_NUMBER_OFFSET].EEPROM_attrib = attrib;
                     
                     uint8_t b[] = "tx.txt=\" :  \"";
                     b[1] = 0x30+(attrib>>ATTRIB_NUMBER_OFFSET)+2;
                     print(b);
                     uint8_t text[] = "vis tx,0";
                     text[5]=0x30+(attrib>>ATTRIB_NUMBER_OFFSET)+2;
                     print(text);

                   } break;
     case FLAG_OFF: {
                     flag = FLAG_CLEAR;
                      switch((attrib & ATTRIB_NUMBER_MASK)>>ATTRIB_NUMBER_OFFSET)
                       {
                        case 0: {
                                 LHALF_CH_PORT &= ~(1<<CH1);
                                 print("vis p0,0");
                                } break;
                        case 1: {
                                 LHALF_CH_PORT &= ~(1<<CH2);
                                 print("vis p1,0");
                                } break;
                        case 2: {
                                 LHALF_CH_PORT &= ~(1<<CH3);
                                 print("vis p2,0");
                                } break;
                        case 3: {
                                 HHALF_CH_PORT &= ~(1<<CH4);
                                 print("vis p3,0");
                                } break;
                        case 4: {
                                 HHALF_CH_PORT &= ~(1<<CH5);
                                 print("vis p4,0");
                                } break;
                        case 5: {
                                 HHALF_CH_PORT &= ~(1<<CH6);
                                 print("vis p5,0");
                                } break;
                        default: break;
                       }
                       ATTRIB_STATE_CLEAR(attrib);
                       EEPROM_channel_data[attrib>>ATTRIB_NUMBER_OFFSET].EEPROM_attrib = attrib;
                       
                       uint8_t b[] = "tx.txt=\" :  \"";
                       b[1] = 0x30+(attrib>>ATTRIB_NUMBER_OFFSET)+2;
                       print(b);
                       uint8_t text[] = "vis tx,0";
                       text[5]=0x30+(attrib>>ATTRIB_NUMBER_OFFSET)+2;
                       print(text);
                       
                     } break;
     case FLAG_POS:{                      // Power On Flag (serve power on state)
                      flag = FLAG_CLEAR;
                      uint8_t num = (attrib>>ATTRIB_NUMBER_OFFSET);
                      uint8_t tmp = EEPROM_channel_data[num].EEPROM_attrib;
                      
                      if(ATTRIB_STATE_IS_SET(tmp)) 
                       {
                        on(IRQ_OUTER, FORCED);
                       }
                      if(ATTRIB_STATE_IS_CLEAR(tmp))
                       {
                        off(IRQ_OUTER);
                       }
                   } break;                
     default: break;
    }
  } else {
          uint32_t t = check_timer(timer);
          uint32_t t1 = t-1;
          t /= 1000UL/TIMERTIC; 
          t1 /= 1000UL/TIMERTIC;
          if(t != t1)
           {
            uint8_t a[] = "vis tx,1";
            a[5] = 0x30+(attrib>>ATTRIB_NUMBER_OFFSET)+2;
            print(a);
            uint8_t b[] = "tx.txt=\" :  \"";
            uint8_t min = t/60;
            uint8_t sec = t-min*60;
            b[1] = 0x30+(attrib>>ATTRIB_NUMBER_OFFSET)+2;
            b[8] = 0x30+min;
            b[10]= 0x30+sec/10;
            b[11]= 0x30+sec%10;
            print(b);
           }
         } 
 }



void channel_t::on(uint8_t src, uint8_t mark)
 {
  //if(ATTRIB_STATE_IS_CLEAR(attrib))   // if status is clear -> then ON 
  { 
   //if(!flag && !ps->flag)  // if channel is not in transition
    {
    switch((attrib & ATTRIB_STATUS_MASK)>>ATTRIB_STATUS_OFFSET)
     {
      case 0: { // single channel ON
                if(!flag)               // if channel isn't transition
                 { 
                  flag = FLAG_ON;
                  init_timer(timer, 0);
                 }
              } break;
      case 1: { // master channel ON
               if(mark == FORCED)     // mandatory ON without state check (if channel is ON then forced ON)
                {
                 if(!flag && !(ps->flag))  // if channel isn't transition
                 { 
                  flag = FLAG_ON;
                    uint8_t text[] = "vis tx,1";
                    text[5]=0x30+(attrib>>ATTRIB_NUMBER_OFFSET)+2;
                    print(text);
                  init_timer(timer, ((hyst*60)*1000UL/TIMERTIC));
                  if(ps!=NULL) ps->on(IRQ_INNER, SMART);
                 }
                } else {  
                        if(ATTRIB_STATE_IS_CLEAR(attrib))     // if channel is OFF then ON
                         {
                          if(!flag && !(ps->flag))  // if channel isn't transition
                           { 
                            flag = FLAG_ON;
                            uint8_t text[] = "vis tx,1";
                            text[5]=0x30+(attrib>>ATTRIB_NUMBER_OFFSET)+2;
                            print(text);
                            init_timer(timer, ((hyst*60)*1000UL/TIMERTIC));
                            if(ps!=NULL) ps->on(IRQ_INNER, SMART);
                           }
                         }
                       }
              } break;          
      case 2: { // slave channel ON
               if(!flag)                // if channel isn't transition
                { 
                 if(src == IRQ_INNER)
                  {
                   flag = FLAG_ON;
                   init_timer(timer, 0);
                  }
                }
              } break;        
      default: break;
     } 
    }
   }
 }


void channel_t::off(uint8_t src)
 {
  //if(ATTRIB_STATE_IS_SET(attrib))   // if state set -> then OFF
  { 
  //if(!flag && !ps->flag)  // if channel is not in transition
   { 
   switch((attrib & ATTRIB_STATUS_MASK)>>ATTRIB_STATUS_OFFSET)
    {
     case 0: { // single channel OFF
               if(!flag)                // if chsnnel isn't transition
                { 
                 flag = FLAG_OFF;
                 init_timer(timer, 0);
                }
             } break;
     case 1: { // master channel OFF
              //if(ATTRIB_STATE_IS_SET(attrib))    // if channel is ON then OFF
               {  
                if(!flag && !(ps->flag))    // if chsnnel isn't transition
                 { 
                  flag = FLAG_OFF;
                  init_timer(timer, 0);
                  if(ps!=NULL) ps->off(IRQ_INNER);
                 }
               }
             } break;          
     case 2: { // slave channel OFF
              if(ATTRIB_STATE_IS_SET(attrib))    // if channel is ON then OFF
              { 
               if(!flag)                     // if chsnnel isn't transition
                { 
                 if(src == IRQ_INNER)
                  {
                     uint8_t text[] = "vis tx,1";
                     text[5]=0x30+(attrib>>ATTRIB_NUMBER_OFFSET)+2;
                     print(text);
                   flag = FLAG_OFF;
                   init_timer(timer, ((hyst*60)*1000UL/TIMERTIC));
                  }
                }
              }
             } break;        
     default: break;
    }
   }
  }
 }


void channel_t::toggle(uint8_t src)
 {
   if(ATTRIB_STATE_IS_SET(attrib)) off(src);
    else on(src, SMART);
 }


void channel_t::on_state(void)
 {
   switch((attrib & ATTRIB_STATUS_MASK)>>ATTRIB_STATUS_OFFSET)
   {
    case 0: { // channel is single
            } break;
    case 1: { // channel is master
              if(ATTRIB_STATE_IS_CLEAR(attrib)) ATTRIB_STATE_CLEAR(ps->attrib);
            } break;
    case 2: { // channel is slave
            } break;
    default: break;
   } 
   if(ATTRIB_STATE_IS_SET(attrib)) on(IRQ_OUTER, SMART);
 }


void channel_t::refresh_buttons(void)
 {
   switch((attrib & ATTRIB_STATUS_MASK)>>ATTRIB_STATUS_OFFSET)
   {
    case 0: { // channel is single
              uint8_t text[] = "vis bx,1";
              text[5]=0x30+(attrib>>ATTRIB_NUMBER_OFFSET);
              print(text);
            } break;
    case 1: { // channel is master
              uint8_t text[] = "vis bx,1";
              text[5]=0x30+(attrib>>ATTRIB_NUMBER_OFFSET);
              print(text);
            } break;
    case 2: { // channel is slave
              uint8_t text[] = "vis bx,0";
              text[5]=0x30+(attrib>>ATTRIB_NUMBER_OFFSET);
              print(text);
            } break;
    default: break;
   }
    
 }

void channel_t::refresh_state(void)
 {
   switch((attrib & ATTRIB_STATE_MASK)>>ATTRIB_STATE_OFFSET)
   {
    case 0: { // channel is OFF
              uint8_t text[] = "vis px,0";
              text[5]=0x30+(attrib>>ATTRIB_NUMBER_OFFSET);
              print(text);
            } break;
    case 1: { // channel is ON
              uint8_t text[] = "vis px,1";
              text[5]=0x30+(attrib>>ATTRIB_NUMBER_OFFSET);
              print(text);
            } break;
    default: break;
   }
 }



void channel_t::refresh_status(void)
 {
   switch((attrib & ATTRIB_STATUS_MASK)>>ATTRIB_STATUS_OFFSET)
    {
     case 0: {
              uint8_t text[] = "t1x.txt=\"I\"";
              text[2]=0x30+(attrib>>ATTRIB_NUMBER_OFFSET);
              //text[9]='I';
              print(text);
             } break;
     case 1: {
              uint8_t text[] = "t1x.txt=\"M\"";
              text[2]=0x30+(attrib>>ATTRIB_NUMBER_OFFSET);
              //text[9]='M';
              print(text);
             } break;
     case 2: {
              uint8_t text[] = "t1x.txt=\"S\"";
              text[2]=0x30+(attrib>>ATTRIB_NUMBER_OFFSET);
              //text[9]='S';
              print(text);
             } break;
     default: break;
    } 
 }


void channel_t::toggle_status(void)
 {
   if(((attrib & ATTRIB_NUMBER_MASK)>>ATTRIB_NUMBER_OFFSET)%2)
    {
      if(ATTRIB_STATUS_IS_SINGLE(attrib)) 
       {
        ATTRIB_STATUS_SET_SLAVE(attrib);
        ATTRIB_STATUS_SET_MASTER(pm->attrib);
        EEPROM_channel_data[attrib>>ATTRIB_NUMBER_OFFSET].EEPROM_attrib = attrib;
        EEPROM_channel_data[pm->attrib>>ATTRIB_NUMBER_OFFSET].EEPROM_attrib = pm->attrib;
       } else
      if(ATTRIB_STATUS_IS_SLAVE(attrib))
       {
        ATTRIB_STATUS_SET_SINGLE(attrib);
        ATTRIB_STATUS_SET_SINGLE(pm->attrib);
        EEPROM_channel_data[attrib>>ATTRIB_NUMBER_OFFSET].EEPROM_attrib = attrib;
        EEPROM_channel_data[pm->attrib>>ATTRIB_NUMBER_OFFSET].EEPROM_attrib = pm->attrib;
       } 
    } 
 }


void channel_t::refresh_hyst(void)
 {
   uint8_t b[] = "tx.txt=\" \"";
   b[1] = 0x30+(attrib>>ATTRIB_NUMBER_OFFSET)+2;
   b[8] = 0x30+hyst;
   print(b);
 }


void channel_t::inc_hyst(void)
 {
   if(hyst<9) hyst++; else hyst=0;
   EEPROM_channel_data[attrib>>ATTRIB_NUMBER_OFFSET].EEPROM_hyst = hyst;
 }


uint8_t channel_t::in_transition(void)
 {
   return check_timer(timer);
 }

//---------------------------------------------------------------------
// Œ¡⁄ﬂ¬À≈Õ»≈: void init_core(uint8_t src)
// Œœ»—¿Õ»≈: 
// œ¿–¿Ã≈“–€:
// ¬€’Œƒ: 
//---------------------------------------------------------------------
void init_core(void)
 {
   LHALF_CH_DDR |= (1<<CH1)|(1<<CH2)|(1<<CH3);
   HHALF_CH_DDR |= (1<<CH4)|(1<<CH5)|(1<<CH6);
   PORTB &= ~((1<<PB0)|(1<<PB1)|(1<<PB2));
   PORTD &= ~((1<<PD5)|(1<<PD6)|(1<<PD7));
   
   for(uint8_t i=0; i<CHANNEL_NUMBER; i++)
    ch[i].time_service_init();
   
   ch[0].ps = &ch[1];
   ch[1].ps = NULL;
   ch[2].ps = &ch[3];
   ch[3].ps = NULL;
   ch[4].ps = &ch[5];
   ch[5].ps = NULL;
   
   ch[0].pm = NULL;
   ch[1].pm = &ch[0];
   ch[2].pm = NULL;
   ch[3].pm = &ch[2];
   ch[4].pm = NULL;
   ch[5].pm = &ch[4];
   
   //for(uint8_t i=0; i<CHANNEL_NUMBER; i++)
   // ch[i].refresh();
   //for(uint8_t i=0; i<CHANNEL_NUMBER; i++)
   // ch[i].on_state();
      
   mode = EEPROM_mode;
   
   for(uint8_t i=0; i<CHANNEL_NUMBER; i++)
    {
      prog[i].inum = EEPROM_ch_prog[i].inum;
      for(uint8_t j=0; j<(MAX_INTERVAL_NUMBER*2); j++)
        prog[i].ch_prog[j] = EEPROM_ch_prog[i].ch_prog[j];
    }
   
   core_en = 1;
 }



void on_state(void)
 {
   for(uint8_t i=0; i<CHANNEL_NUMBER; i++)
    ch[i].on_state();
 }


void refresh_buttons(void)
 {
   for(uint8_t i=0; i<CHANNEL_NUMBER; i++)
    ch[i].refresh_buttons();
 }

void refresh_state(void)
 {
   for(uint8_t i=0; i<CHANNEL_NUMBER; i++)
    ch[i].refresh_state();
 }

void refresh_status(void)
 {
   for(uint8_t i=0; i<CHANNEL_NUMBER; i++)
    ch[i].refresh_status();
 }

void refresh_hyst(void)
 {
   for(uint8_t i=0; i<CHANNEL_NUMBER; i++)
    ch[i].refresh_hyst();
 }

void poll_core(void)
 {
   for(uint8_t i=0; i<CHANNEL_NUMBER; i++)
    ch[i].poll(); 
 }



void poll_hmi(void)
 {
   if(time.ready_to_show)
     {
      uint8_t d[] = "t1.txt=\"00:00\""; 
      uint8_t m0, m1, h0, h1;
      
      m0 = time.min%10;
      m1 = time.min/10;
      h0 = time.hour%10;
      h1 = time.hour/10;
      d[8]=0x30+h1;
      d[9]=0x30+h0;
      d[11]=0x30+m1;
      d[12]=0x30+m0;
      
      print(d); 
      time.ready_to_show = 0;
     }
 }


void poll_prog()
 {
   if(time.ready_to_pollprog)
    {
     uint16_t dtime = time.hour*60 + time.min;
      
      
     for(uint8_t k=0; k<CHANNEL_NUMBER; k++)
      {
       uint8_t r;
       for(r=0; r<(EEPROM_ch_prog[k].inum*2); r++)
        {
          if(r==(EEPROM_ch_prog[k].inum*2-1))    // last time point
           {
             if(EEPROM_ch_prog[k].ch_prog[r]==0 && EEPROM_ch_prog[k].ch_prog[0]==0)    // if last and first iqual zero
              {
                if(dtime < 1440) break;
              } else {
                       if(dtime < EEPROM_ch_prog[k].ch_prog[r]) break;
                     }
           } else {
                    if(dtime < EEPROM_ch_prog[k].ch_prog[r]) break;
                  }
        }
       if(r%2==0)
        {
         ch[k].off(IRQ_OUTER);
        } else {
                ch[k].on(IRQ_OUTER, SMART);
               }
      }
       
      
            
     time.ready_to_pollprog = 0;  
    }
 }


void set_mode_auto()
 {
   EEPROM_mode = MODE_AUTO;
   mode = MODE_AUTO;
 }

void set_mode_manual()
 {
   EEPROM_mode = MODE_MANUAL;
   mode = MODE_MANUAL;
 }

void switch_mode()
 {
   if(mode==MODE_MANUAL) print("page 1");
   if(mode==MODE_AUTO) print("page 2");
 }

void upload_program(uint8_t channel)
 {
  download_ch = channel;                       //if want to save after - in to this channel 
  char str1[]="page1.n.val= ";                 // upload interval num. value to Nextion variable page1.n.val
  str1[12]=0x30+EEPROM_ch_prog[channel].inum;
  print((uint8_t const *)str1);
  
  char str2[21]="page1.timeX.val=    ";
  for(int i=0; i<INTERVAL_NUMBER; i++)
  { 
   str2[10]=0x30+(i+1);          // place number of channel 
   uint16_t time = EEPROM_ch_prog[channel].ch_prog[i];
   if(time>=0 && time<10) 
    { 
      str2[16]=0x30+time;
      str2[17]='\0';
    }
   if(time>=10 && time<100)
    {
      str2[16]=0x30+time/10;
      str2[17]=0x30+time%10;
      str2[18]='\0';
    }
   if(time>=100 && time<1000)
    {
      str2[18]=0x30+time%10; time=time/10;
      str2[17]=0x30+time%10; time=time/10;
      str2[16]=0x30+time%10; 
      str2[19]='\0';
    } 
   if(time>=1000 && time<=1440)
    {
      str2[19]=0x30+time%10; time=time/10;
      str2[18]=0x30+time%10; time=time/10;
      str2[17]=0x30+time%10; time=time/10;
      str2[16]=0x30+time%10; 
      str2[20]='\0';
    }
   print((uint8_t const *)str2);
  }
 }

void edit_program(uint8_t sign, uint16_t data)
 {
   switch(sign)
    {
     case HMI_RETURN_SIGN_0: {
                              EEPROM_ch_prog[download_ch].inum = data;
                             } break;
     case HMI_RETURN_SIGN_1: {
                              EEPROM_ch_prog[download_ch].ch_prog[0] = data;
                             } break;
     case HMI_RETURN_SIGN_2: {
                              EEPROM_ch_prog[download_ch].ch_prog[1] = data;
                             } break;
     case HMI_RETURN_SIGN_3: {
                              EEPROM_ch_prog[download_ch].ch_prog[2] = data;
                             } break;
     case HMI_RETURN_SIGN_4: {
                              EEPROM_ch_prog[download_ch].ch_prog[3] = data;
                             } break;
     case HMI_RETURN_SIGN_5: {
                              EEPROM_ch_prog[download_ch].ch_prog[4] = data;
                             } break;
     case HMI_RETURN_SIGN_6: {
                              EEPROM_ch_prog[download_ch].ch_prog[5] = data;
                             } break;
     case HMI_RETURN_SIGN_7: {
                              EEPROM_ch_prog[download_ch].ch_prog[6] = data;
                             } break;
     case HMI_RETURN_SIGN_8: {
                              EEPROM_ch_prog[download_ch].ch_prog[7] = data;
                             } break;
     default: break;                        
    } 
 }