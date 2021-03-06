//-------------------------------------------------
// ������ ������ � ������ ��������� �������        |
//  pcf8583.�                                      | 
// ������:     ���� �������                        |
// ����������: ICC AVR v6.30B                      |
// ���������:  pcf8583 (RTC with RAM)              |
// ����:       ATmega8535                          |
// �����:      ���������� �.�.                     |
// �����:      19.07.07                            |
// �����:      -                                   | 
//-------------------------------------------------
#include "defs.h"
#include "pcf8583.h"
#include "i2c.h"

tTime time;
tDate date;

/*extern char* monthname[] = {   
"������",
"�������",
"�����",
"������",
"���",
"����",
"����",
"�������",
"�������",
"�������",
"������",
"�������"                   
};*/  

void init_rtc(void)
 {
   DDRD &= ~(1<<PD3);        // PD3 as input 
   EICRA |= (1<<ISC11);      // falling adge generate interrupt   
   EIMSK |= (1<<INT1);       // enable INT1 interrupt
   init_i2c();
 }

//----------------------------------------------------------------------
//DISCRIPTION:  unsigned char bin_to_bcd( unsigned char bin )
//              ������� � �������� ��������� �������� ����� (����) � �������� �������
//              � ��������� ��� � ������ BCD (�������-������������ ����������). 
//PARAMETERS:   bin - ����� � �������� �������.
//RETURN VALUE: ����� (����) � BCD �������.
//----------------------------------------------------------------------
uint8_t bin_to_bcd(uint8_t bin)
 {
  uint8_t bcdh = 0x00, bcdl = 0x00;
  while(bin>=10) {bin -= 10; bcdh++;}
  bcdl = bin;
  return((bcdh<<4)|bcdl);
 }


//----------------------------------------------------------------------
//DISCRIPTION:  unsigned char bcd_to_bin( unsigned char bcd )
//              ������� � �������� ��������� �������� ����� (����) � BCD �������
//              � ��������� ��� � �������� ������. 
//PARAMETERS:   bcd - ����� � ������� BCD.
//RETURN VALUE: ����� (����) � �������� �������.
//---------------------------------------------------------------------- 
uint8_t bcd_to_bin(uint8_t bcd)
 {
  return((bcd>>4)*10+(bcd&0x0f));
 } 
 
 
//----------------------------------------------------------------------
//DISCRIPTION:  void rtc_get_time(tTime *time) 
//              ������� ��������� ��������� ���� tTime ������ ���������� 
//PARAMETERS:   time - ���������� ���� tTime.  
//RETURN VALUE: ---
//---------------------------------------------------------------------- 
void rtc_get_time(tTime* time)
 {
  //time->hsec = bcd_to_bin(read_byte(PCF8583, 0x0001));
  time->sec  = bcd_to_bin(read_byte(PCF8583, 0x0002));
  time->min  = bcd_to_bin(read_byte(PCF8583, 0x0003));
  time->hour = bcd_to_bin(read_byte(PCF8583, 0x0004)&0x3f);
  /*if(time->sec == 0)*/ time->ready_to_show = 1;            // every second
  if(time->sec == 0) time->ready_to_pollprog = 1;            // every minute
 }
 
 
//----------------------------------------------------------------------
//DISCRIPTION:  void rtc_set_time(tTime time) 
//              ������� ���������� �������� ����� ��������� ���� tTime � PCF8583 (RTC)
//              (������������� �����) 
//PARAMETERS:   time - ���������� ���� tTime.  
//RETURN VALUE: ---
//----------------------------------------------------------------------
void rtc_set_time(tTime time)
 {
  uint8_t tmp;
  tmp = read_byte(PCF8583, 0x0000);   //tmp <- status reg.
  tmp |= 0x80;                            //set stop counting flag
  write_byte(PCF8583, 0x0000, tmp);   //write tmp to pcf8583
   //counting is stopped now!!!
   
  //write_byte(PCF8583, 0x0001, bin_to_bcd(time.hsec));
  write_byte(PCF8583, 0x0002, bin_to_bcd(time.sec));
  write_byte(PCF8583, 0x0003, bin_to_bcd(time.min)); 
  write_byte(PCF8583, 0x0004, bin_to_bcd(time.hour));
  
  tmp &= 0x7f;                            //
  write_byte(PCF8583, 0x0000, tmp);   //start count again 
 }
 
 
//----------------------------------------------------------------------
//DISCRIPTION:  void rtc_get_date(tDate *date) 
//              ������� ��������� ��������� ���� tDate ������ ���������� 
//PARAMETERS:   date - ���������� ���� tDate.  
//RETURN VALUE: ---
//---------------------------------------------------------------------- 
void rtc_get_date(tDate* date)
 {
  uint8_t tmp = read_byte(PCF8583, 0x0006);          //read weekday and month
  //date->weekday = (tmp>>5);
  date->month   = bcd_to_bin(tmp&0x1f);
   tmp = read_byte(PCF8583, 0x0005);                       //read year and day
  date->year    = (tmp>>6);
  date->day     = bcd_to_bin(tmp&0x3f);
  
  tmp = read_byte(PCF8583,0x0010);
  date->year += tmp;   
 }
 
 
//----------------------------------------------------------------------
//DISCRIPTION:  void rtc_set_date(tDate date) 
//              ������� ���������� �������� ����� ��������� ���� tDate � PCF8583 (RTC)
//              (������������� ����) 
//PARAMETERS:   date - ���������� ���� tDate.  
//RETURN VALUE: ---
//----------------------------------------------------------------------
void rtc_set_date(tDate date)
 {
  uint8_t tmp, rest, base;
  rest=date.year;
  tmp = read_byte(PCF8583, 0x0000);   //tmp <- status reg.
  tmp |= 0x80;                            //set stop counting flag
  write_byte(PCF8583, 0x0000, tmp);   //write tmp to pcf8583
   //counting is stopped now!!!
  
  while(rest>=4) rest-=4;
  base=date.year-rest;
   write_byte(PCF8583, 0x0005, bin_to_bcd(date.day)|(rest<<6)); //write day/year register
   write_byte(PCF8583, 0x0006, bin_to_bcd(date.month)|0x00/*(date.weekday<<5)*/); //write month/weekday register
   write_byte(PCF8583, 0x0010, base);
   write_byte(PCF8583, 0x0011, rest);
    
  tmp &= 0x7f;                            //
  write_byte(PCF8583, 0x0000, tmp);   //start count again
 }    
 


//---------------------------------------------------------------------
// ����������: void INT1_interrupt( void )
// ��������: 
// ���������:
// �����:
//---------------------------------------------------------------------
#pragma vector = INT1_vect
__interrupt void INT1_interrupt( void )
 {
   rtc_get_time(&time);
 } 