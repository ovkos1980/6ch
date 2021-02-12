//-------------------------------------------------
// Модуль работы с часами реального времени        |
//  pcf8583.h                                      | 
// проект:     Рэле времени                        |
// компилятор: ICC AVR v6.30B                      |
// перефирия:  pcf8583 (RTC with RAM)              |
// ядро:       ATmega8535                          |
// автор:      Овчинников К.В.                     |
// старт:      19.07.07                            |
// финиш:      -                                   | 
//-------------------------------------------------



//аппаратные адреса
#define I2CEEPROM 0xa0
#define PCF8583   (I2CEEPROM|0x00) /*A0 -> GND*/
#define AT24C64   (I2CEEPROM|0x04) /*A0 -> GND; A1 -> +5V; A2 -> GND*/

//тип время
typedef struct
  {
   uint8_t ready_to_show;
   uint8_t ready_to_pollprog;
   //unsigned char hsec;
   uint8_t sec;
   uint8_t min;
   uint8_t hour;
  } tTime;

//тип дата  
typedef struct
  {
   //unsigned char weekday;
   uint8_t day;
   uint8_t month;
   uint8_t year;  //[00..99]
  } tDate;  

//extern 
extern tTime time;
extern tDate date;
  
//прототипы функций для работы с PCF8583 (RTC)  
void init_rtc(void);
//unsigned char rtc_get_status();
void rtc_get_time(tTime *time);
void rtc_set_time(tTime time);
void rtc_get_date(tDate *date);
void rtc_set_date(tDate date);

