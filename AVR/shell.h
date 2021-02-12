#ifndef _SHELL_H
#define _SHELL_H

#include <stdint.h>

// max packet length 
#define MAX_PKT_LENGTH       32

// command shell state
#define PACKET_WAIT_STATE    0x00
#define PACKET_RECEIVE_STATE 0x02
#define PACKET_PROCESS_STATE 0x03
#define PACKET_RELEASE_STATE 0x04
#define PACKET_SENDANS_STATE 0x05

// interface functions prototypes
void init_shell(void);
void poll_pkt(void);
void print(uint8_t const *);

#endif //_SHELL_H