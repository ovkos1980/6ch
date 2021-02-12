#ifndef _DEFS_H
#define _DEFS_H

#include <iom168pa.h>
#include <ina90.h>
#include <avr_macros.h>
#include <stdint.h>
#include <stdio.h>
//#include <stdlib.h>

//#define BQ1  //7.3728 MHz
//#define BQ2  //8.0000 MHz
//#define BQ3  //11.059 MHz 
#define BQ4  //14.7456 MHz

#ifdef BQ1
 #define F_CLK  7372800UL   //7.3728 MHz
#endif
#ifdef BQ2
 #define F_CLK  8000000UL   //8.0 MHz
#endif 
#ifdef BQ3
 #define F_CLK  11059200UL  //11.059 MHz 
#endif
#ifdef BQ4
 #define F_CLK  14745600UL  //14.7456 MHz 
#endif

#endif //_DEFS_H
