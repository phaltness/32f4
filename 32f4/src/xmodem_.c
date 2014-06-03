/*******************************************************************************
 *
 * Filename: xmodem.c
 *
 * Instantiation of simple xmodem support using uart channel.
 *
 ******************************************************************************/

#include "stdio.h"
#include "stm32f4xx.h"
//#include "main.h"
//#include "stm32f4xx_flash.h"

/* Line control codes */
#define SOH			0x01	/* start of header for Xmodem protocol  */
#define STX			0x02	/* start of header for 1K xmodem protocol*/
#define ACK			0x06	/* Acknowledge */
#define NAK			0x15	/* Negative acknowledge */
#define CAN			0x18	/* Cancel */
#define EOT			0x04	/* end of text */
#define TIMEOUT 				2
#define TIMEOUT_LONG 			10
#define XMODEM_DATA_SIZE_SOH 	128  /* for Xmodem protocol */
#define XMODEM_DATA_SIZE_STX 	1024 /* for 1K xmodem protocol */
#define FLASH_PAGE_SIZE         ((uint16_t)0x1000)   // 1KB

enum {
	INVALID_STATE = 0, WAITING_START, WAIT_HEAD, WAIT_NUMBER, WAIT_NOTNUMBER, RX_PACKET, RX_EOT, SEND_NAK
};

static char data_buf[1029];
static char packetNumber;
static int PACKET_SIZE;
static char HEAD;
u8 *pbuf, store[10240];
u8 *head_store = 0;

#define TransitionState(x, y) (x = y)

int WaitForChar(char *cPtr, uint32_t seconds);
extern void error(int numerror);
extern void Delay(__IO uint32_t nTime);

//extern void FLASH_Configuration(void);
//extern FLASH_Status FLASH_Erase_Sector(unsigned int StartAddr,
//		unsigned int Length);
//extern FLASH_Status FLASH_Write_Word(unsigned int Address, unsigned int Data);
//extern unsigned int FLASH_Read_Word(unsigned int Address);

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
static uint32_t GetRecord(char blocknum, uint32_t dest) {
	int size;
	char nextChar;
	unsigned chk, j;
	volatile FLASH_Status FLASHStatus;
	u8 src;

	chk = 0;

//	if (head_store){
//		head_store = &store[0];
//	}
	if (!WaitForChar(&nextChar, TIMEOUT)) {
		return (0);
	}

	if ((char) nextChar != (char) ~blocknum) {
		return (0);
	}

	for (size = 0; size < PACKET_SIZE; size++) {
		if (!WaitForChar(&nextChar, TIMEOUT)) {
			return (0);
		}
		chk = chk ^ (uint32_t) nextChar << 8;
		for (j = 0; j < 8; ++j) {
			if (chk & 0x8000)
				chk = chk << 1 ^ 0x1021;
			else
				chk = chk << 1;
		}
		//	*dest++ = nextChar;
		data_buf[size] = nextChar;
	}

	chk &= 0xFFFF;

	if ((!WaitForChar(&nextChar, TIMEOUT))
			|| (nextChar != ((chk >> 8) & 0xFF))) {
		return (0);
	}

	if ((!WaitForChar(&nextChar, TIMEOUT)) || (nextChar != (chk & 0xFF))) {
		return (0);
	}

//  fram write
//	pbuf = &data_buf;
//
//	for(size=0;size < PACKET_SIZE;)
//	{
//		*(store+size)=*(pbuf+size);
//		size++;
//	}

//	Delay(5);
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
int xmodem_rx(uint32_t dest) {

	int number, notnumber, state;
	char nextChar;
	uint32_t startAddress = dest;

	packetNumber = 1;
	state = WAITING_START;
	PACKET_SIZE = XMODEM_DATA_SIZE_SOH;		//=128byte
	HEAD = SOH;

	while (1) {

		if (state == RX_PACKET) {
			if (!WaitForChar(&nextChar, TIMEOUT)) {
				error(1);
				return (-1);
			}

			if (GetRecord(packetNumber, dest)) {
				dest += PACKET_SIZE;
				++packetNumber;
				TransitionState(state, WAIT_HEAD);
			} else {
				error(2);
				return (-1);
			}
		}

		if (state == WAITING_START) {
			DebugPutc('C');
			if (WaitForChar(&nextChar, TIMEOUT)) {
				HEAD = nextChar;
				if (HEAD == SOH) {
					PACKET_SIZE = XMODEM_DATA_SIZE_SOH;
					HEAD = SOH;
					TransitionState(state, WAIT_NUMBER);
				} else if (HEAD == STX) {
					PACKET_SIZE = XMODEM_DATA_SIZE_STX;
					HEAD = STX;
					TransitionState(state, WAIT_NUMBER);
				}
			}
		}

		if (state == WAIT_NUMBER) {
			if (WaitForChar(&nextChar, TIMEOUT)) {
				if (nextChar != packetNumber) {
					// TransitionState(state, SEND_NAK);
					DebugPutc(NAK);
					error(4);
					return (-1);
				} else {
					TransitionState(state, RX_PACKET);
				}
			}
		}

//		if (state == WAIT_NOTNUMBER) {
//			if (WaitForChar(&nextChar, TIMEOUT)) {
//				if (nextChar != (packetNumber ^ 0xFF)) {
//					// TransitionState(state, SEND_NAK);
//					DebugPutc(NAK);
//					error(4);
//					return (-1);
//				} else {
//					TransitionState(state, RX_PACKET);
//				}
//			}
//		}

		if (state == WAIT_HEAD) {
			if (!WaitForChar(&nextChar, TIMEOUT_LONG)) {
				error(8);
				return (-1);
			}

			/*			if (nextChar == HEAD) {
			 TransitionState(state, RX_PACKET);
			 }
			 */
			if (nextChar == SOH) {
				PACKET_SIZE = XMODEM_DATA_SIZE_SOH;
				HEAD = SOH;
				TransitionState(state, RX_PACKET);
			} else if (nextChar == STX) {
				PACKET_SIZE = XMODEM_DATA_SIZE_STX;
				HEAD = STX;
				TransitionState(state, RX_PACKET);
			}

			if (nextChar == EOT) {
				// TransitionState(state, RX_EOT);
				DebugPutc(ACK);
				return (dest - startAddress);
			}
		}

	}

	// the loop above should return in all cases
	//return (-1);
}
int WaitForChar(char *cPtr, uint32_t seconds) {
	unsigned long counter = 0;

	seconds *= 500; /* delay 200 * 5 * 1ms */

	while (((USART3->SR & USART_FLAG_RXNE) == 0) && (counter++ < seconds)) {
		Delay(1);
	}
	if ((USART3->SR & USART_FLAG_RXNE) != 0) {
		*cPtr = (uint8_t) USART3->DR;
		return 1;
	}
	return 0;
}
