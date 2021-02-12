#include "shell.h"
#include "hmi.h"
#include "defs.h"
#include "uart.h"
#include "core.h" 
#include "pcf8583.h"


//prototypes of inner functions
void dummy(uint8_t *);
void hmi_ssuccess_start(uint8_t *);
void touch(uint8_t *);
void numeric_return(uint8_t *);
void string_return(uint8_t *);
void var_name_invalid(uint8_t *);
void parse(uint8_t *);

// packet data type
typedef struct 
 {
   uint8_t data[MAX_PKT_LENGTH];       // packet data array
   uint8_t cnt;                        // byte receive counter
   uint8_t state;                      // packet receiving state 
   uint8_t suffix;                     // suffix sign
 } packet_t;

// shell structure
struct shell_t
 {
   packet_t packet;                    // shell packet structure
   uint8_t  lock;                      // shell lock flag
   uint8_t  data_sign;                 // sign of data returned from HMI
 } shell;

// command -> function pointer structure
struct ptentry 
 {
  uint8_t first_byte;                  // first byte of returned data
  void (* pfunc)(uint8_t *);           // pointer to according handler function 
 };

// parse table
static struct ptentry parsetab[] =
 {{HMI_TOUCH_EVENT,      touch},
  {HMI_NUMERIC_VAR_DATA, numeric_return},
  {HMI_STRING_VAR_DATA,  string_return},
  {HMI_START_SUCCESS,    hmi_ssuccess_start},
  {HMI_VAR_NAME_INVALID, var_name_invalid},
  {HMI_UNKNOWN,          dummy},
};



//---------------------------------------------------------------------
// Œ¡⁄ﬂ¬À≈Õ»≈: void dummy(uint8_t *p)
// Œœ»—¿Õ»≈:
// œ¿–¿Ã≈“–€:
// ¬€’Œƒ:
//---------------------------------------------------------------------
void dummy(uint8_t *p)
 {
   //while(!UART_tx_ready());
   //UART_put_byte(0xCC);
   //UART_flush_buffer();
 }

void print(uint8_t const *cmd)
 {
   uint8_t suffix[] = {0xFF, 0xFF, 0xFF, 0x00};  
   uint8_t const *p = cmd;
   
   while(!UART_tx_ready());
   while(*p != 0) UART_put_byte(*p++);
   p = suffix;
   while(*p != 0) UART_put_byte(*p++);
   UART_flush_buffer();
 }

void hmi_ssuccess_start(uint8_t *pdata)
 {
   //print("page 1");
   hmi_en = 1;
   
   
   /*for(int i=0; i<6; i++)
     ch[i].refresh(1);*/
 }


void touch(uint8_t *pdata)
 {
  if(pdata[1] == 1)   // touch event from page 1
   { 
    switch(pdata[2])
     {
     case HMI_BT1_ID: { ch[0].toggle(IRQ_OUTER); } break;
     case HMI_BT2_ID: { ch[1].toggle(IRQ_OUTER); } break;
     case HMI_BT3_ID: { ch[2].toggle(IRQ_OUTER); } break;
     case HMI_BT4_ID: { ch[3].toggle(IRQ_OUTER); } break;
     case HMI_BT5_ID: { ch[4].toggle(IRQ_OUTER); } break;
     case HMI_BT6_ID: { ch[5].toggle(IRQ_OUTER); } break;
     case HMI_P_MODE_ID: { 
                           uint8_t tmp = 0;
                           for(int i=0; i<6; i++)
                            if(ch[i].in_transition()) tmp=1;
                           
                           if(!tmp)  // if no one channel in transition
                           { 
                            print("page 2");
                            set_mode_auto();
                            refresh_buttons();
                            refresh_state();
                            time.ready_to_pollprog=1/*poll_prog()*/;
                           }
                         } break; 
     case HMI_P_SETTINGS_ID: { 
                              uint8_t tmp = 0;
                              for(int i=0; i<6; i++)
                               if(ch[i].in_transition()) tmp=1;
                              
                              if(!tmp)  //if no one channel in transition
                               {
                                print("page 3");
                                refresh_status();
                                refresh_hyst();
                               }
                             } break;
      case HMI_P_TIME_SET_ID: {
                               print("page 4");
                              } break;                       
      default: break;
     }
   }
  if(pdata[1] == 2)    // touch event from page 2
   {
    switch(pdata[2])
     {
     case HMI_P_MODE_ID: { 
                           uint8_t tmp = 0;
                           for(int i=0; i<6; i++)
                            if(ch[i].in_transition()) tmp=1;
                           
                           if(!tmp) // if no one channel in transition
                            {  
                             print("page 1");
                             set_mode_manual();
                             refresh_buttons();
                             refresh_state(); 
                            } 
                         } break; 
     default: break;
     }
   }
  if(pdata[1] == 3)    // touch event from page 3
   {
     switch(pdata[2])
      {
       case HMI_BT1_ID: { ch[0].toggle_status(); refresh_status(); } break;
       case HMI_BT2_ID: { ch[1].toggle_status(); refresh_status(); } break;
       case HMI_BT3_ID: { ch[2].toggle_status(); refresh_status(); } break;
       case HMI_BT4_ID: { ch[3].toggle_status(); refresh_status(); } break;
       case HMI_BT5_ID: { ch[4].toggle_status(); refresh_status(); } break;
       case HMI_BT6_ID: { ch[5].toggle_status(); refresh_status(); } break;
       default: break;
      }
       
     switch(pdata[2])
      {
       case HMI_HYST1_ID: { ch[0].inc_hyst(); refresh_hyst(); } break;
       case HMI_HYST2_ID: { ch[1].inc_hyst(); refresh_hyst(); } break;
       case HMI_HYST3_ID: { ch[2].inc_hyst(); refresh_hyst(); } break;
       case HMI_HYST4_ID: { ch[3].inc_hyst(); refresh_hyst(); } break;
       case HMI_HYST5_ID: { ch[4].inc_hyst(); refresh_hyst(); } break;
       case HMI_HYST6_ID: { ch[5].inc_hyst(); refresh_hyst(); } break;
       case HMI_P3_EXIT_ID: { 
                             print("page 1"); 
                             refresh_buttons();
                             refresh_state(); 
                            } break;
       default: break;
      }
     
     switch(pdata[2])
      {
       case HMI_PROG1_BTN_ID: {
                               upload_program(0);
                               print("page 5"); 
                              } break;
       case HMI_PROG2_BTN_ID: { 
                               upload_program(1);
                               print("page 5"); 
                              } break;
       case HMI_PROG3_BTN_ID: {
                               upload_program(2);
                               print("page 5"); 
                              } break;
       case HMI_PROG4_BTN_ID: {
                               upload_program(3);
                               print("page 5"); 
                              } break;
       case HMI_PROG5_BTN_ID: {
                               upload_program(4); 
                               print("page 5");
                              } break;
       case HMI_PROG6_BTN_ID: {
                               upload_program(5);
                               print("page 5"); 
                              } break;
       default: break;
      }
       //refresh_status();
       //refresh_hyst();
   } 
  if(pdata[1] == 4)    // touch event from page 4
   {
    switch(pdata[2])
     {
      case HMI_TIME_ENTER_ID: { 
                               //print("get page1.time.val");
                               //refresh_buttons();
                               //refresh_state();
                              } break;
      case HMI_TIME_EXIT_ID: {
                              print("page 1");  
                              refresh_buttons();
                              refresh_state();
                              refresh_hyst();
                             } break;
      default: break;
     }
   }
  if(pdata[1] == 5)    // touch event from page 5
   {
    switch(pdata[2])
     {
      case HMI_P5_EXIT_ID: {
                            print("page 3");
                            refresh_status();
                            refresh_hyst();
                           } break;
      default: break;
     } 
   }
 }




void numeric_return(uint8_t *pdata)
 {
   switch(shell.data_sign)
   {
    case HMI_RETURN_SIGN_T: {// time for store
                             uint16_t dc_time = 0;
                             dc_time = pdata[1] + (pdata[2]<<8);
                             uint8_t h = dc_time/60, m = dc_time - h*60;
                             time.hour = h;
                             time.min = m;
                             time.sec = 0;
                             rtc_set_time(time);
                             print("page 1");
                             refresh_buttons();
                             refresh_state();
                             refresh_hyst();
                            } break;
    case HMI_RETURN_SIGN_0: 
    case HMI_RETURN_SIGN_1: 
    case HMI_RETURN_SIGN_2: 
    case HMI_RETURN_SIGN_3: 
    case HMI_RETURN_SIGN_4: 
    case HMI_RETURN_SIGN_5: 
    case HMI_RETURN_SIGN_6: 
    case HMI_RETURN_SIGN_7: 
    case HMI_RETURN_SIGN_8: {
                             uint16_t d = pdata[1] + (pdata[2]<<8);
                             edit_program(shell.data_sign, d);
                             if(shell.data_sign == HMI_RETURN_SIGN_8) 
                              {
                               print("page 3");
                               refresh_status();
                               refresh_hyst();
                              }
                            } break;                        
    default: break;
   }
  shell.data_sign = HMI_RETURN_SIGN_NULL; 
 }



void string_return(uint8_t *pdata)
 {
   shell.data_sign = pdata[1];
 }



void var_name_invalid(uint8_t *pdata)
 {
   //print("dim=20");
 }

//---------------------------------------------------------------------
// Œ¡⁄ﬂ¬À≈Õ»≈: void null_packet(void)
// Œœ»—¿Õ»≈:
// œ¿–¿Ã≈“–€:
// ¬€’Œƒ:
//---------------------------------------------------------------------
void null_packet(void)
 {
   shell.packet.cnt            = 0; 
   shell.packet.suffix         = 0;
   shell.packet.state          = PACKET_WAIT_STATE;
   shell.lock                  = 0;  
   //UART_rx_unlock();                                
 }



//---------------------------------------------------------------------
// Œ¡⁄ﬂ¬À≈Õ»≈: void init_shell(void)
// Œœ»—¿Õ»≈:
// œ¿–¿Ã≈“–€:
// ¬€’Œƒ:
//---------------------------------------------------------------------
void init_shell(void)
 {
   null_packet();
   shell.data_sign = HMI_RETURN_SIGN_NULL;
 }



//---------------------------------------------------------------------
// Œ¡⁄ﬂ¬À≈Õ»≈: void poll_pkt(void)
// Œœ»—¿Õ»≈:
// œ¿–¿Ã≈“–€:
// ¬€’Œƒ:
//---------------------------------------------------------------------
void poll_pkt(void)
 {
   switch(shell.packet.state)
    {
     case PACKET_WAIT_STATE: {
                              if(UART_rx_ready())                      // if UART RX buffer lock and ready to export data 
                               {                                       //  and shell unlock and read to process packet
                                if(UART_data_in_rx_buffer())           //  transfer data from RX UART buffer to shell buffer while data in UART buffer 
                                 {
                                  uint8_t data = UART_get_byte(); 
                                  shell.packet.data[shell.packet.cnt] = data;
                                  shell.packet.cnt++;
                                   if(shell.packet.data[shell.packet.cnt-1] == 0xFF &&
                                      shell.packet.data[shell.packet.cnt-2] == 0xFF)
                                     shell.packet.suffix++;
                                   if(shell.packet.suffix == 2) shell.packet.state = /*PACKET_RECEIVE_STATE*/PACKET_PROCESS_STATE;
                                 } else {
                                         UART_rx_unlock();
                                         shell.packet.state = /*PACKET_RECEIVE_STATE*/PACKET_PROCESS_STATE;
                                        }
                               }  
                             } break;
     case PACKET_RECEIVE_STATE: {
                                 //shell.packet.state = PACKET_PROCESS_STATE;
                                } break;
     case PACKET_PROCESS_STATE: {
                                 if(shell.packet.data[shell.packet.cnt-1]==0xFF &&
                                    shell.packet.data[shell.packet.cnt-2]==0xFF &&
                                    shell.packet.data[shell.packet.cnt-3]==0xFF)
                                  {
                                    shell.packet.state = PACKET_RELEASE_STATE;
                                  } else {
                                          null_packet();  
                                         }
                                } break;
     case PACKET_RELEASE_STATE: {
                                 parse(shell.packet.data);
                                 //shell.packet.state = PACKET_SENDANS_STATE;
                                 null_packet();
                                } break;
     case PACKET_SENDANS_STATE: {
                                 /*uint8_t i=0;
                                 while(i<shell.packet.cnt)
                                  {
                                    UART_put_byte(shell.packet.data[i++]);
                                  }*/
                                 /*if(UART_tx_ready())
                                  { 
                                   UART_put_byte(0xDD);
                                   UART_flush_buffer();
                                   null_packet();
                                  }*/
                                  null_packet();
                                } break;
     default: null_packet(); break;
    }
 }



//---------------------------------------------------------------------
// Œ¡⁄ﬂ¬À≈Õ»≈: void parse(uint8_t *pdata)
// Œœ»—¿Õ»≈:
// œ¿–¿Ã≈“–€:
// ¬€’Œƒ: 
//---------------------------------------------------------------------
void parse(uint8_t *pdata)
 {
  uint8_t i=0;
  while(parsetab[i].first_byte != HMI_UNKNOWN)
   {
    if(parsetab[i].first_byte == pdata[0]) 
     { 
       parsetab[i].pfunc(pdata);   // found command in list so call corresponding function
       return;
     }
    i++;
   }
  parsetab[i].pfunc(pdata);  // no one function found in table so call FB_UNKNOWN corresponding function
 }
