#include "defs.h"
#include "uart.h"


struct uart_t
 {
   uint8_t rx_buffer[UART_RX_BUFFER_SIZE];   // RX buffer
   volatile uint8_t rx_head;                 // RX buffer head pointer 
   volatile uint8_t rx_tail;                 // RX buffer tail pointer 
   uint8_t tx_buffer[UART_TX_BUFFER_SIZE];   // TX buffer
   volatile uint8_t tx_head;                 // TX buffer head pointer 
   volatile uint8_t tx_tail;                 // TX buffer tail pointer 
   uint8_t rx_wr_lock;                       // RX buffer filled with data and ready to read 
   uint8_t tx_wr_lock;                       // TX buffer lock for write procedure flag
 } UART; 


//---------------------------------------------------------------------
// ÎÁÚßÂËÅÍÈÅ: void clear_UART_buffer(void)
// ÎÏÈÑÀÍÈÅ: Function clear RX and TX UART buffers, set to initial values 
//           auxiliary variables.
// ÏÀÐÀÌÅÒÐÛ:
// ÂÛÕÎÄ:
//---------------------------------------------------------------------
void clear_UART_buffer(void)
 {
   UART.rx_tail = 0;
   UART.rx_head = 0;
   UART.tx_tail = 0;
   UART.tx_head = 0;
   UART.tx_wr_lock = 0;
   UART.rx_wr_lock = 0;
 }



//---------------------------------------------------------------------
// ÎÁÚßÂËÅÍÈÅ: void init_UART(uint32_t baud)
// ÎÏÈÑÀÍÈÅ: Function enable hardvare UART, flush buffers and auxilary variables
// ÏÀÐÀÌÅÒÐÛ:
// ÂÛÕÎÄ:
//---------------------------------------------------------------------
void init_UART(uint32_t baud)
 {
  uint16_t UBRR_val;
  
  UART.rx_tail = 0;
  UART.rx_head = 0;
  UART.tx_tail = 0;
  UART.tx_head = 0;
  UART.tx_wr_lock = 0;
  UART.rx_wr_lock = 0;
   
  UBRR_val = F_CLK/16/baud-1;             // calculate UBRR value
  UBRR0H = (uint8_t)(UBRR_val>>8);        // Set baud rate 
  UBRR0L = (uint8_t)UBRR_val; 	          
  UART_CTRL_PORT &= ~(1<<UART_RSE_PIN);  // 485 RSE pin as input (receive direction)
  UCSR0B = ((1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0)|(1<<TXCIE0)); // Enable UART receiver, transmitter, receive interrupt, 
                                                            // transmitter interrupt
  // timeout timer 
  TCCR0B |= ((1<<CS02)|(1<<CS00));                     // clk/1024 ( ~ 0..17.7 ms)
  TCNT0 = 0xFF-(UART_TIMEOUT*F_CLK)/(1024UL*1000UL);   // value for compare
  TIMSK0 |= (1<<TOIE0);                                // overflow interrupt enable 
 }



//---------------------------------------------------------------------
// ÎÁÚßÂËÅÍÈÅ: void UART_rx_interrupt( void )
// ÎÏÈÑÀÍÈÅ: 
// ÏÀÐÀÌÅÒÐÛ:
// ÂÛÕÎÄ:
//---------------------------------------------------------------------
#pragma vector = USART_RX_vect
__interrupt void UART_rx_interrupt( void )
{
  uint8_t data;
  uint8_t tmphead;
   
  if(!UART.rx_wr_lock)                                 // if RX buffer unlock to write
   { 
    TCNT0 = 0xFF-(UART_TIMEOUT*F_CLK)/(1024UL*1000UL); // restart timer/counter0
    TCCR0B |= ((1<<CS02)|(1<<CS00));                   // start timer/counter at clk/1024
      
    data = UDR0;                                        // Read the received data
    tmphead = (UART.rx_head+1)&UART_RX_BUFFER_MASK;    // Calculate buffer index 
    if(tmphead == UART.rx_tail)                        // if owerflow appear handle error
     {
      UART.rx_wr_lock = 1;
      TCCR0B &= ~((1<<CS02)|(1<<CS00));                 // stop timer/counter0
      // ERROR! Receive buffer overflow
     } else {                                          // there are no owerflow condition
             UART.rx_head = tmphead;                   // Store new index 
             UART.rx_buffer[tmphead] = data;           // Store received data in buffer
            }
   } else {
           data = UDR0;                                 // dummy read 
          }
 }



//---------------------------------------------------------------------
// ÎÁÚßÂËÅÍÈÅ: void UART_rx_timeout_interrupt( void )
// ÎÏÈÑÀÍÈÅ: 
// ÏÀÐÀÌÅÒÐÛ:
// ÂÛÕÎÄ:
//---------------------------------------------------------------------
#pragma vector = TIMER0_OVF_vect
__interrupt void UART_rx_timeout_interrupt( void )
 {
   if(UART_data_in_rx_buffer())
    {
     //LEDON
     UART.rx_wr_lock = 1;
    }
   TCCR0B &= ~((1<<CS02)|(1<<CS00));                    // stop timer/counter0
 }



//---------------------------------------------------------------------
// ÎÁÚßÂËÅÍÈÅ: uint8_t UART_get_byte( void )
// ÎÏÈÑÀÍÈÅ: Get one byte data from RX buffer filled by interrupt
// ÏÀÐÀÌÅÒÐÛ:
// ÂÛÕÎÄ:
//---------------------------------------------------------------------
uint8_t UART_get_byte( void )
{ 
  unsigned char tmptail;

  if(UART.rx_head != UART.rx_tail);                       // if there are incomming data 
  tmptail = ( UART.rx_tail + 1 ) & UART_RX_BUFFER_MASK;   // Calculate buffer index 
  UART.rx_tail = tmptail;                                 // Store new index 
  return UART.rx_buffer[tmptail];                         // Return data 
} 



//---------------------------------------------------------------------
// ÎÁÚßÂËÅÍÈÅ: uint8_t UART_data_in_rx_buffer( void )
// ÎÏÈÑÀÍÈÅ: 
// ÏÀÐÀÌÅÒÐÛ:
// ÂÛÕÎÄ:
//---------------------------------------------------------------------
uint8_t UART_data_in_rx_buffer( void )
{
  return (UART.rx_head != UART.rx_tail);                  // Return 0 (FALSE) if the receive buffer is empty
}



//---------------------------------------------------------------------
// ÎÁÚßÂËÅÍÈÅ: uint8_t UART_put_byte(uint8_t)
// ÎÏÈÑÀÍÈÅ: Put byte to tx buffer. If there are place in buffer and buffer
//           not busy, function put data and will return 0. If buffer is busy
//           or owerflowed - function will return 1
// ÏÀÐÀÌÅÒÐÛ:
// ÂÛÕÎÄ:
//---------------------------------------------------------------------
uint8_t UART_put_byte(uint8_t data)
 {
  unsigned char tmphead;
  
  if(!UART.tx_wr_lock)                                       // if TX buffer unlock to write
   { 
    tmphead = ( UART.tx_head + 1 ) & UART_TX_BUFFER_MASK;    // Calculate buffer index 
    if( tmphead == UART.tx_tail )
     {
       return 1;
      // ERROR! Receive buffer overflow 
     } else {
             UART.tx_head = tmphead;                         // Store new index 
             UART.tx_buffer[UART.tx_head] = data;            // Store pointed data in buffer 
             return 0;
            }
   } else return 1;
 }



//---------------------------------------------------------------------
// ÎÁÚßÂËÅÍÈÅ: void UART_flush_buffer(void)
// ÎÏÈÑÀÍÈÅ: Flush TX UART buffer (transmitt all data) 
// ÏÀÐÀÌÅÒÐÛ:
// ÂÛÕÎÄ:
//---------------------------------------------------------------------
void UART_flush_buffer(void)
 {
   UART.tx_wr_lock = 1;                      // forbid store to TX buffer
   UART_CTRL_PORT |= (1<<UART_RSE_PIN);      // 485 converter set to transmitt mode
   UCSR0B |= (1<<UDRIE0);                    // Enable UDRE interrupt
 }



//---------------------------------------------------------------------
// ÎÁÚßÂËÅÍÈÅ: void UART_register_empty_interrupt( void )
// ÎÏÈÑÀÍÈÅ:
// ÏÀÐÀÌÅÒÐÛ:
// ÂÛÕÎÄ:
//---------------------------------------------------------------------
#pragma vector = USART_UDRE_vect
__interrupt void UART_register_empty_interrupt( void )
{
  unsigned char tmptail;
  
  
  if(UART.tx_head != UART.tx_tail)                          // Check if all data is transmitted 
   {
    tmptail = ( UART.tx_tail + 1 ) & UART_TX_BUFFER_MASK;   // Calculate buffer index 
    UART.tx_tail = tmptail;                                 // Store new index 
    UDR0 = UART.tx_buffer[tmptail];                         // Start transmition 
   } else {
	   UCSR0B &= ~(1<<UDRIE0);                          // Disable UDRE interrupt
           UART_CTRL_PORT &= ~(1<<UART_RSE_PIN);            // 485 converter set to receive mode          
           UART.tx_wr_lock = 0;                             // unlock TX buffer for write
	  }
}



//---------------------------------------------------------------------
// ÎÁÚßÂËÅÍÈÅ: uint8_t UART_rx_ready( void )
// ÎÏÈÑÀÍÈÅ:
// ÏÀÐÀÌÅÒÐÛ:
// ÂÛÕÎÄ:
//---------------------------------------------------------------------
uint8_t UART_rx_ready(void)
 {
   return UART.rx_wr_lock;
 }



//---------------------------------------------------------------------
// ÎÁÚßÂËÅÍÈÅ: uint8_t UART_tx_ready( void )
// ÎÏÈÑÀÍÈÅ:
// ÏÀÐÀÌÅÒÐÛ:
// ÂÛÕÎÄ:
//---------------------------------------------------------------------
uint8_t UART_tx_ready(void)
 {
   return !UART.tx_wr_lock;
 }



//---------------------------------------------------------------------
// ÎÁÚßÂËÅÍÈÅ: void UART_rx_unlock( void )
// ÎÏÈÑÀÍÈÅ:
// ÏÀÐÀÌÅÒÐÛ:
// ÂÛÕÎÄ:
//---------------------------------------------------------------------
void UART_rx_unlock(void)
 { 
   UART.rx_wr_lock = 0;
 }
