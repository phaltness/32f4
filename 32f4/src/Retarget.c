/*----------------------------------------------------------------------------
 * Name:    Retarget.c
 * Purpose: 'Retarget' layer for target-dependent low level functions
 * Note(s):
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2012 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <stdio.h>
//#include <rt_misc.h>

//#pragma import(__use_no_semihosting_swi)


extern int  myputchar(int c);                               /* see uart.c */
extern int  mygetchar(void);                                /* see uart.c */

#ifdef TRACEUART
extern int  traceputchar(int c);                               /* see uart.c */
extern int  tracegetchar(void);                                /* see uart.c */
#endif //TRACEUART

#ifdef TRACEUART
#endif //TRACEUART


struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;
FILE __stderr;


int
fputc(int c, FILE *f) {
  if (c == '\n')  {
    myputchar('\r');
  }
#ifdef TRACEUART
  traceputchar(c);
#endif// TRACEUART
  return (myputchar(c));
}

int
fgetc(FILE *f) {
#ifdef TRACEUART
	return (traceputchar(mygetchar()));
#else// TRACEUART
	return (mygetchar());
#endif// TRACEUART
}

#ifdef TRACEUART
int fputd(int c) {
  if (c == '\n')  {
    traceputchar('\r');
  }
  return (traceputchar(c));
}
int fgetd(void) {
  return (tracegetchar());
}
#endif// TRACEUART

//int ferror(FILE *f) {
  /* Your implementation of ferror */
//  return EOF;
//}


void _ttywrch(int c) {
  myputchar(c);
}


void _sys_exit(int return_code) {
label:  goto label;  /* endless loop */
}
