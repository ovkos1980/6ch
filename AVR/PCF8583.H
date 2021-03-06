//-------------------------------------------------
// ������ ������ � ������ ��������� �������        |
//  pcf8583.h                                      | 
// ������:     ���� �������                        |
// ����������: ICC AVR v6.30B                      |
// ���������:  pcf8583 (RTC with RAM)              |
// ����:       ATmega8535                          |
// �����:      ���������� �.�.                     |
// �����:      19.07.07                            |
// �����:      -                                   | 
//-------------------------------------------------



//���������� ������
#define I2CEEPROM 0xa0
#define PCF8583   (I2CEEPROM|0x00) /*A0 -> GND*/
#define AT24C64   (I2CEEPROM|0x04) /*A0 -> GND; A1 -> +5V; A2 -> GND*/

//��� �����
typedef struct
  {
   uint8_t ready_to_show;
   uint8_t ready_to_pollprog;
   //unsigned char hsec;
   uint8_t sec;
   uint8_t min;
   uint8_t hour;
  } tTime;

//��� ����  
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
  
//��������� ������� ��� ������ � PCF8583 (RTC)  
void init_rtc(void);
//unsigned char rtc_get_status();
void rtc_get_time(tTime *time);
void rtc_set_time(tTime time);
void rtc_get_date(tDate *date);
void rtc_set_date(tDate date);

