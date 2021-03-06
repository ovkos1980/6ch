//-------------------------------------------------
// ������ ���������� I2C                           |
//  I2C.h                                          | 
// ������:     ���� �������                        |
// ����������: ICC AVR v6.30B                      |
// ����:       -                                   |
// �����:      ���������� �.�.                     |
// �����:      19.07.07                            |
// �����:      -                                   | 
//-------------------------------------------------


//------------------------------------------------------------------------------
#define I2C_PORT      PORTC
#define I2C_DDR       DDRC
#define I2C_PIN       PINC
#define SCL           5
#define SDA           4

#define I2C_CLK       80000     // i2c clock in Hz (max value for this project is 100 kHz) 

//��������� �������
void init_i2c(void);
void i2c_error(void);
unsigned char read_byte(unsigned char devaddr, unsigned int wordaddr);
void write_byte(unsigned char devaddr, unsigned int wordaddr, unsigned char data);