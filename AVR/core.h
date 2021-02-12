#ifndef _CORE_H
#define _CORE_H

// class declaration
class channel_t
 {
   uint8_t attrib;
   uint8_t hyst;
   uint8_t flag;
   uint8_t timer;
  public: 
   channel_t(uint8_t);
   void time_service_init(void);
   void poll(void);
   void on_state(void);
   void refresh_buttons(void);
   void refresh_state(void);
   void refresh_status(void);
   void toggle_status(void);
   void refresh_hyst(void);
   void inc_hyst(void);
   void on(uint8_t src, uint8_t mark);
   void off(uint8_t src);
   void toggle(uint8_t src);
   uint8_t in_transition(void);
   channel_t *ps;                        // pointer to slave
   channel_t *pm;                        // pointer to master
 };


extern uint8_t mode;
extern uint8_t hmi_en;
extern uint8_t core_en;
extern channel_t ch[6];



#define CHANNEL_NUMBER              6
#define INTERVAL_NUMBER             8

// channel ports
#define LHALF_CH_DDR                DDRB
#define LHALF_CH_PORT               PORTB
#define HHALF_CH_DDR                DDRD
#define HHALF_CH_PORT               PORTD

// channel lines
#define CH1                         PB2  
#define CH2                         PB1
#define CH3                         PB0
#define CH4                         PD7
#define CH5                         PD6
#define CH6                         PD5

// EEPROM attributes start address
#define EEPROM_ATTRIB_ADDRESS       0x10
#define EEPROM_MODE_ADDRESS         0x1E
#define EEPROM_CH_PROG_ADDRESS      0x30

#define MAX_INTERVAL_NUMBER         4

// channel attributes
#define ATTRIB_STATE_OFFSET         0
#define ATTRIB_STATE_MASK           (0x01<<ATTRIB_STATE_OFFSET)
#define ATTRIB_STATE_CLEAR(x)       (x &= ~ATTRIB_STATE_MASK)
#define ATTRIB_STATE_SET(x)         (x |= ATTRIB_STATE_MASK)
#define ATTRIB_STATE_IS_SET(x)      ((x & ATTRIB_STATE_MASK)>>ATTRIB_STATE_OFFSET)
#define ATTRIB_STATE_IS_CLEAR(x)    !((x & ATTRIB_STATE_MASK)>>ATTRIB_STATE_OFFSET)

#define ATTRIB_STATUS_OFFSET        1
#define ATTRIB_STATUS_MASK          (0x03<<ATTRIB_STATUS_OFFSET)
#define ATTRIB_STATUS_SET_SINGLE(x) {x &= ~(ATTRIB_STATUS_MASK);}
#define ATTRIB_STATUS_SET_MASTER(x) {x &= ~(ATTRIB_STATUS_MASK); x |= (1<<ATTRIB_STATUS_OFFSET);}
#define ATTRIB_STATUS_SET_SLAVE(x)  {x &= ~(ATTRIB_STATUS_MASK); x |= (1<<ATTRIB_STATUS_OFFSET+1);}
#define ATTRIB_STATUS_IS_SINGLE(x)  !(x & ATTRIB_STATUS_MASK)
#define ATTRIB_STATUS_IS_MASTER(x)  ((x & ATTRIB_STATUS_MASK)>>ATTRIB_STATUS_OFFSET == 1)
#define ATTRIB_STATUS_IS_SLAVE(x)   ((x & ATTRIB_STATUS_MASK)>>ATTRIB_STATUS_OFFSET == 2)

#define ATTRIB_MODE_OFFSET          3
#define ATTRIB_MODE_MASK            (0x01<<ATTRIB_MODE_OFFSET)
#define ATTRIB_MODE_MANUAL(x)       (x &= ~ATTRIB_MODE_MASK)
#define ATTRIB_MODE_AUTO(x)         (x |= ATTRIB_MODE_MASK)
#define ATTRIB_MODE_IS_MANUAL(x)    !((x & ATTRIB_MODE_MASK)>>ATTRIB_MODE_OFFSET)
#define ATTRIB_MODE_IS_AUTO(x)      ((x & ATTRIB_MODE_MASK)>>ATTRIB_MODE_OFFSET)

#define ATTRIB_NUMBER_OFFSET        4
#define ATTRIB_NUMBER_MASK          (0xF<<ATTRIB_NUMBER_OFFSET)
#define ATTRIB_NUMBER(x)            (x >> ATTRIB_NUMBER_OFFSET)

#define CHANNEL_SINGLE              0
#define CHANNSL_MASTER              1
#define CHANNEL_SLAVE               2 

// flags
#define FLAG_CLEAR                  0
#define FLAG_ON                     1                     
#define FLAG_OFF                    2
#define FLAG_POS                    3       // flag Power ON State

#define SMART                       0       // mark for ON channel procedure
#define FORCED                      1

// Implementation request 
#define IRQ_INNER                   0  // request from microprocessor
#define IRQ_OUTER                   1  // request from HMI

// relay mode
#define MODE_MANUAL                 0
#define MODE_AUTO                   1

// interface function prototypes
void init_core(void);
void poll_core(void);
void poll_hmi(void);
void poll_prog(void);
void on_state(void);
void refresh_buttons(void);
void refresh_state(void);
void refresh_status(void);
void refresh_hyst(void);
void on(uint8_t, uint8_t);
void set_mode_auto(void);
void set_mode_manual(void);
void switch_mode(void);
void upload_program(uint8_t);
void edit_program(uint8_t, uint16_t);

#endif //_CORE_H