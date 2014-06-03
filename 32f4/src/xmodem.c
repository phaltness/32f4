/*******************************************************************************
 *
 * Filename: xmodem.c
 *
 * Instantiation of simple xmodem support using uart channel.
 *
 ******************************************************************************/

#include "stdio.h"
#include "stm32f4xx.h"
#include "stm32f4xx_crc.h"
#include "xmodem.h"

/* Line control codes */
#define SOH			0x01	/* start of header for Xmodem protocol  */
#define STX			0x02	/* start of header for 1K xmodem protocol*/
#define ACK			0x06	/* Acknowledge */
#define NAK			0x15	/* Negative acknowledge */
#define CAN			0x18	/* Cancel */
#define EOT			0x04	/* end of text */
#define TIMEOUT 				3
#define TIMEOUT_LONG 			10
#define XMODEM_DATA_SIZE_SOH 	128  /* for Xmodem protocol */
#define XMODEM_DATA_SIZE_STX 	1024 /* for 1K xmodem protocol */
#define FLASH_PAGE_SIZE         ((uint16_t)0x400)   // 1KB

enum {
	INVALID_STATE = 0,
	WAITING_START,
	WAIT_HEAD,
	RX_PACKET,
	RX_EOT,
	SEND_NAK
};

static char	uart_buf[1029] = {0,0};
static char *ptx_uart_buf, *prx_uart_buf = &uart_buf[0];
char *head_uart_buf = &uart_buf[0];
static u8 	uartbufferNE = 0;
static char	data_buf[1024];
static char	packetNumber;
static int	PACKET_SIZE;
static char	HEAD;

u8 			store[10240] = {0,0};

#define TransitionState(x, y) (x = y)

int WaitForChar(char *cPtr, int seconds); 
extern void Delay(__IO uint32_t nTime);
//extern void FLASH_Configuration(void);
//extern FLASH_Status FLASH_Erase_Sector(unsigned int StartAddr,unsigned int Length);
//extern FLASH_Status FLASH_Write_Word(unsigned int Address,unsigned int Data);
//extern unsigned int FLASH_Read_Word(unsigned int Address);
#ifdef TRACEUART
extern int traceprintf(const char *format, ...);
#endif //TRACEUART

/*
 * .KB_C_FN_DEFINITION_START
 *  This global function writes a character to the debug uart port as soon
 * as it is ready to send another character.
 * .KB_C_FN_DEFINITION_END
 */
void DebugPutc(char cValue) {
	
	myputchar(cValue & 0x1FF);

}

/*
 * .KB_C_FN_DEFINITION_START
 *  This private function receives a x-modem record to the pointer and
 * returns non-zero on success.
 * .KB_C_FN_DEFINITION_END
 */
static int GetRecord(char blocknum, int dest) {
	int					size;
	char				nextChar;
	uint16_t			chk, j;
//    volatile FLASH_Status FLASHStatus;

	chk = 0;

	if (!WaitForChar(&nextChar, TIMEOUT)) {
		return (0);
	}

	if ((char)nextChar != (char)~blocknum) {
#ifdef TRACEUART
		traceprintf("receive %d for !packetnumber %d\n\r", nextChar, (char)~blocknum);
#endif //TRACEUART
		return (0);
	}

	chk = 0;
	for (size = 0; size < PACKET_SIZE; size++) {
		if (!WaitForChar(&nextChar, TIMEOUT)) {
#ifdef TRACEUART
			traceprintf("timeout in receiving of %x byte of packet\n\r", size);
#endif //TRACEUART
			return (0);
		}
		chk = chk ^ ((int)nextChar << 8);
		for (j = 0; j < 8; ++j) {
			if (chk & 0x8000)
				chk = (chk << 1) ^ 0x1021;
			else
				chk = chk << 1;

			chk &= 0xFFFF;
		}
        data_buf[size] = nextChar;
	}

	if ((!WaitForChar(&nextChar, TIMEOUT)) || (nextChar != ((chk >> 8) & 0xFF))) {
#ifdef TRACEUART
		traceprintf("\nreceived notEqual CRC1 %d != %d\n\r", nextChar, (((chk >> 8) & 0xFF)));
#endif //TRACEUART
		return (0);
	}

	if ((!WaitForChar(&nextChar, TIMEOUT)) || (nextChar != (chk & 0xFF))) {
#ifdef TRACEUART
		traceprintf("\nreceived notEqual CRC0 %d != %d\n\r", nextChar, (chk & 0xFF));
#endif //TRACEUART
		return (0);
	}
    
//  buf write
	while(uartbufferNE) {
	}

	for(size=0;size < PACKET_SIZE;size++)
    {
        *(u8*)(dest+size) = *(u8*)(data_buf+size);
    }

	ptx_uart_buf = &uart_buf[0];
    prx_uart_buf = &uart_buf[0];
	DebugPutc(ACK);

	return (1);
}


/* ************************** GLOBAL FUNCTIONS ********************************/


/*
 * .KB_C_FN_DEFINITION_START
 *  This global function receives a x-modem transmission consisting of
 * (potentially) several blocks.  Returns the number of bytes received or
 * -1 on error.
 * .KB_C_FN_DEFINITION_END
 */
int xmodem_rx(unsigned int dest) {

	int		state;
	char	nextChar;
	int 	count = 0;
	int 	startAddress = dest;

	prx_uart_buf = ptx_uart_buf = &uart_buf[0];
	packetNumber = 1;
	state = WAITING_START;
	PACKET_SIZE = XMODEM_DATA_SIZE_SOH;
//	HEAD = SOH;
						
	while (count<30) {

		if (state == WAITING_START) {
			DebugPutc('C');
			count++;
			if (WaitForChar(&nextChar, TIMEOUT)) 
            {
                HEAD=nextChar;
				if (HEAD == SOH) 
				{
						PACKET_SIZE = XMODEM_DATA_SIZE_SOH;
						HEAD = SOH;
						TransitionState(state, RX_PACKET);
#ifdef TRACEUART
						traceprintf("SOH detected\n\r");
#endif //TRACEUART
				}
				else
				if	(HEAD == STX) 
				{
						PACKET_SIZE = XMODEM_DATA_SIZE_STX;
						HEAD = STX;
						TransitionState(state, RX_PACKET);
#ifdef TRACEUART
						traceprintf("STH detected\n\r");
#endif //TRACEUART
				}
			}
		}

		if (state == WAIT_HEAD) {
			if (!WaitForChar(&nextChar, TIMEOUT_LONG)) {
				return (-1);
			}

/*			if (nextChar == HEAD) {
				TransitionState(state, RX_PACKET);
			}
*/
				if (nextChar == SOH) 
				{
						PACKET_SIZE = XMODEM_DATA_SIZE_SOH;
						HEAD = SOH;
						TransitionState(state, RX_PACKET);
				}
				else
				if	(nextChar == STX) 
				{
						PACKET_SIZE = XMODEM_DATA_SIZE_STX;
						HEAD = STX;
						TransitionState(state, RX_PACKET);
				}
				
			if (nextChar == EOT) {
				DebugPutc(ACK);
				USART_ITConfig(USART3,USART_IT_RXNE,DISABLE);
				return (dest - startAddress);
			}
		}

		if (state == RX_PACKET) {
			if (!WaitForChar(&nextChar, TIMEOUT)) {
				return (-1);
			}

			if (nextChar != packetNumber) {
				DebugPutc(NAK);
#ifdef TRACEUART
				traceprintf("return error %d:%d\n\r", packetNumber, nextChar);
#endif //TRACEUART
				return (-1);
			} else {
			}

			if (GetRecord(packetNumber, dest)) {
				dest += PACKET_SIZE;
				++packetNumber;
				TransitionState(state, WAIT_HEAD);
			} else {
				return (-1);
			}
		}
	}

	// the loop above should return in all cases
	return (-1);
}
int WaitForChar(char *cPtr, int seconds) 
{
  	unsigned long counter = 0;
  	static int i;
  	
  	seconds *= 500 ;  /* delay 200 * 5 * 1ms */
  	
  	while (!uartbufferNE && (counter++ < seconds))
    {
		Delay (2);
    }
    if (uartbufferNE)
    {
      *cPtr = *prx_uart_buf;
      if (++prx_uart_buf >= ptx_uart_buf)
    	  {uartbufferNE = 0;}
      return 1;
    }
    return 0;
}

//static int count = 0;

void
USART3_IRQHandler(void)
{
	if (USART3->SR & USART_FLAG_RXNE)
	{
		*ptx_uart_buf = (uint8_t)(USART3->DR);
		ptx_uart_buf++;
		uartbufferNE = 1;
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	}
}
