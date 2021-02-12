#ifndef _UART_H
#define _UART_H

/* main defenitions */
#define UART_DATA_PORT  PORTD
#define UART_DATA_DDR   DDRD
#define UART_DATA_PIN   PIND
#define UART_CTRL_PORT  PORTD
#define UART_CTRL_DDR   DDRD
#define UART_CTRL_PIN   PIND

#define UART_RX_PIN     0
#define UART_TX_PIN     1
#define UART_RSE_PIN    2

// UART timeout 
#define UART_TIMEOUT    10         // 10 ms

// UART Buffer Defines 
#define UART_RX_BUFFER_SIZE 128    // 2,4,8,16,32,64,128 or 256 bytes 
#define UART_TX_BUFFER_SIZE 128    // 2,4,8,16,32,64,128 or 256 bytes 

#define UART_RX_BUFFER_MASK ( UART_RX_BUFFER_SIZE - 1 )
#if ( UART_RX_BUFFER_SIZE & UART_RX_BUFFER_MASK )
#error RX buffer size is not a power of 2
#endif

#define UART_TX_BUFFER_MASK ( UART_TX_BUFFER_SIZE - 1 )
#if ( UART_TX_BUFFER_SIZE & UART_TX_BUFFER_MASK )
#error TX buffer size is not a power of 2
#endif

// Function prototypes 
void clear_UART_buffer(void);
void init_UART(uint32_t);
uint8_t UART_get_byte(void);
uint8_t UART_data_in_rx_buffer(void);
uint8_t UART_put_byte(uint8_t);
void UART_flush_buffer(void);
uint8_t UART_rx_ready(void);
uint8_t UART_tx_ready(void);
void UART_rx_unlock(void); 


/*void UART_tx_byte( unsigned char data );
unsigned char UART_data_in_tx_buffer(void);
void UART_put_in_rx_buffer(unsigned char data);
void UART_put_in_tx_buffer(unsigned char data);
void UARTtx( void );*/

#endif //_UART_H
