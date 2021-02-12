//-------------------------------------------------
// Модуль интерфейса I2C                           |
//  I2C.с                                          | 
// проект:     Рэле времени                        |
// компилятор: ICC AVR v6.30B                      |
// ядро:       ATmega8535                          |
// автор:      Овчинников К.В.                     |
// старт:      19.07.07                            |
// финиш:      -                                   | 
//-------------------------------------------------
#include "defs.h"
#include "I2C.h"



//---------------------------------------------------------------------
// ОБЪЯВЛЕНИЕ: void init_I2C( void )
// ОПИСАНИЕ: функция настраивает перрефирию для реализации интерфейса I2C
// ПАРАМЕТРЫ: void
// ВЫХОД: void
//---------------------------------------------------------------------
void init_i2c(void)
 {
   I2C_DDR  &= ~((1<<SCL)|(1<<SDA));   // SCL, SDA линии настроить на ввод
   TWSR |= (1<<TWPS0);                 // SCL clk = (CPU clk)/(16 + 2(TWBR)*4^(TWPS))  -> [TWPS=1] ->  
   TWBR = F_CLK/(8*I2C_CLK)-2;         // -> TWBR = 1/8*F_CLK/I2C_CLK-2 
 }



//----------------------------------------------------------------------
//DISCRIPTION:  void i2c_error( void )
//              
//PARAMETERS:   -
//RETURN VALUE: -
//---------------------------------------------------------------------- 
void i2c_error(void)
 {} 
 
 

//----------------------------------------------------------------------
//DISCRIPTION:  unsigned char read_byte( unsigned char devaddr, 
//                                       unsigned int wordaddr )
//              Функция возвращает байт данных хранящихся по адресу wordaddr в  
//              чипе devaddr.                 
//PARAMETERS:   devaddr - адрес чипа подсоединенного к шине I2C;
//              wordaddr - адрес где хранятся искомые данные.  
//RETURN VALUE: Байт данных.
//----------------------------------------------------------------------
unsigned char read_byte(unsigned char devaddr, unsigned int wordaddr)
 {
  unsigned char data;
             
  _CLI();
  //Режим - "Мастер передатчик"
  TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);  //Sent START condition
   while(!(TWCR&(1<<TWINT)));              //wait for TWINT flag is set
   if((TWSR&0xf8)!=0x08) i2c_error();      //START transmitted OK, else error!
  TWDR = devaddr|0x00;                     //load SLA+W
  TWCR = (1<<TWINT)|(1<<TWEN);             //Sent SLA+W to I2C
   while(!(TWCR&(1<<TWINT)));              //wait for TWINT flag is set
   if((TWSR&0xf8)!=0x18) i2c_error();      //if not ACK then error!
   /*if(devaddr==AT24C64)                    //если выбран чип AT24c64 
   {
    TWDR = wordaddr>>8;                    //load MSB ADDR 
    TWCR = (1<<TWINT)|(1<<TWEN);           //Sent ADDR to I2C
     while(!(TWCR&(1<<TWINT)));            //wait for TWINT flag is set
     if((TWSR&0xf8)!=0x28) i2c_error();    //data was transmitted ACK was recieved else error!
   }*/
  TWDR = wordaddr;                         //load LSB ADDR 
  TWCR = (1<<TWINT)|(1<<TWEN);             //Sent ADDR to I2C
   while(!(TWCR&(1<<TWINT)));              //wait for TWINT flag is set
   if((TWSR&0xf8)!=0x28) i2c_error();      //data was transmitted ACK was recieved else error!
  
  //Режим - "Мастер приемник"
  TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);  //Sent REPEATED START condition
   while(!(TWCR&(1<<TWINT)));              //wait for TWINT flag is set
   if((TWSR&0xf8)!=0x10) i2c_error();      //START transmitted OK, else error!
  TWDR = devaddr|0x01;                     //load SLA+R
  TWCR = (1<<TWINT)|(1<<TWEN);             //Sent SLA+R to I2C
   while(!(TWCR&(1<<TWINT)));              //wait for TWINT flag is set
   if((TWSR&0xf8)!=0x40) i2c_error();      //if not ACK then error!
  TWCR = (1<<TWINT)|(1<<TWEN)|(0<<TWEA);   //Received data and sent NACK to I2C
   while(!(TWCR&(1<<TWINT)));              //wait for TWINT flag is set
   if((TWSR&0xf8)!=0x58) i2c_error();      //if not NACK then error!
  data = TWDR;                             //read data
  TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);  //Sent STOP to I2C
  return(data);
  _SEI();  
 }


 
//----------------------------------------------------------------------
//DISCRIPTION:  void write_byte(unsigned char devaddr, 
//                              unsigned int wordaddr,
//                              unsigned char data) 
//              Функция записывает байт данных по адресу wordaddr в один 
//              из чипов devaddr.                 
//PARAMETERS:   devaddr - адрес чиа подсоединенного к шине I2C;
//              wordaddr - адрес где хранятся искомые данные;
//              data - байт данных для записи.  
//RETURN VALUE: ---
//---------------------------------------------------------------------- 
void write_byte(unsigned char devaddr, unsigned int wordaddr, unsigned char data)
 {
  _CLI();
  //Режим - "Мастер передатчик"
  TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);  //Sent START condition
   while(!(TWCR&(1<<TWINT)));              //Wait for TWINT flag is set
   if((TWSR&0xf8)!=0x08) i2c_error();      //START transmitted OK, else error!
  TWDR = devaddr|0x00;                     //Load SLA+W
  TWCR = (1<<TWINT)|(1<<TWEN);             //Sent SLA+W to I2C
   while(!(TWCR&(1<<TWINT)));              //Wait for TWINT flag is set
   if((TWSR&0xf8)!=0x18) i2c_error();      //If not acknowlage then error!
   /*if(devaddr==AT24C64)                    //если выбран чип AT24c64 
   {
    TWDR = wordaddr>>8;                    //load MSB ADDR 
    TWCR = (1<<TWINT)|(1<<TWEN);           //Sent ADDR to I2C
     while(!(TWCR&(1<<TWINT)));            //wait for TWINT flag is set
     if((TWSR&0xf8)!=0x28) i2c_error();    //data was transmitted ACK was recieved else error!
   }*/
  TWDR = wordaddr;                         //Load LSB address 
  TWCR = (1<<TWINT)|(1<<TWEN);             //Sent address to I2C
   while(!(TWCR&(1<<TWINT)));              //Wait for TWINT flag is set
   if((TWSR&0xf8)!=0x28) i2c_error();      //Data was transmitted ACK was recieved else error!
   
  TWDR = data;                             //Load DATA
  TWCR = (1<<TWINT)|(1<<TWEN);             //Sent data to I2C
   while(!(TWCR&(1<<TWINT)));              //Wait for TWINT flag is set
   if((TWSR&0xf8)!=0x28) i2c_error();      //Data was transmitted ACK was recieved else error!
  TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);  //Sent STOP to I2C
  _SEI(); 
 } 