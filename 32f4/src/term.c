#include <string.h>
#include <stdio.h>                /* prototype declarations for I/O functions */
#include "stm32f4xx.h"


#define MAX_CMDBUF_SIZE		    64
#define MAX_PROMPT_LEN	        1
#define CTL_CH(c)		        ((c) - 'a' + 1)


void getcmd(char *, unsigned char);
void serial_term(void);
extern void Delay(__IO uint32_t nTime);
extern int mygetchar (void);
extern int myputchar (int c);

char prompt[MAX_PROMPT_LEN] = "";

extern	void exec_string(char *);

unsigned char awaitkey(unsigned char timeout)
{
    unsigned char	c;

    while ((USART3->SR & USART_FLAG_RXNE) == 0)
    {
		myputchar(0x08);	/* go backwards */
        printf("\r\b%01d",timeout);

        if (!timeout)
            return 0 ;
        timeout--;

        Delay(1000);
    }
	c = mygetchar();

    return (c);
}

void getcmd(char *cmd_buf, unsigned char len)
{
	char curpos = 0;	/* current position - index into cmd_buf */
	char c;
	int cmd_echo = 1;	//0;

	/* Clear out the buffer */
	memset(cmd_buf, 0, len);

	for (;;) {
		c = mygetchar();
		switch (c) {
		case 0x08:
		case 0x06:
		case 0x07:
		case 0x7E:
		case 0x7F:	/* backspace or delete */
			/* we're not at the beginning of the line */
			if (curpos) {
				curpos--;
				myputchar(0x08);	/* go backwards */
				myputchar(' ');	/* overwrite the char */
				myputchar(0x08);	/* go back again */
			}
			cmd_buf[curpos] = '\0';
			break;
		case '\r':
		case '\n':
		case '\0':
			myputchar('\r');
			myputchar('\n');
			goto end_cmd;
		case CTL_CH('x'):
			curpos = 0;
			break;

		default:
			if (curpos < len-1) {
				cmd_buf[curpos] = c;
				/* echo it back out to the screen */
				if (cmd_echo)
					myputchar(c);
				curpos++;
			}
			break;
		}
	}
end_cmd:
	;
//	printf("COMMAND: %s\n\r", cmd_buf);
}
void serial_term(void)
{
    char cmd_buf[MAX_CMDBUF_SIZE];

	printf("\n\rType \"help\" for help.\n\r");
    for (;;) 
    {
        printf("%s>", prompt);

        getcmd(cmd_buf, MAX_CMDBUF_SIZE);	/*defined in src/getcmd.c*/

        /* execute a user command */
        if (cmd_buf[0])
            exec_string(cmd_buf);
    }
}
