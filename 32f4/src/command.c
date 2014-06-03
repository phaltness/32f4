#include "hwdefs.h"
#include "command.h"
#include <string.h>
#include <stdio.h>                /* prototype declarations for I/O functions */
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "xmodem.h"
#include "spi_loader.h"
#include "main.h"
#include "waveplayer.h"

#define 	NULL	0

static user_command_t *head_cmd = NULL;
static user_command_t *tail_cmd = NULL;

extern user_command_t rx_cmd;
extern user_command_t go_cmd;

//extern char *head_uart_buf;
char *puart_buf;
u8 *length_store = 0;


//extern char	packetNumber;
//extern int PACKET_SIZE;
extern char	store[];

extern int atox(const char *_S);
extern void run_app(uint32_t ApplicationAddress);
extern int xmodem_rx(unsigned int address);
extern void lattice_one_load(uint8_t *pBinary, u32 length);
//long paramoldvalue;
//int argc;

enum ParseState {
	PS_WHITESPACE,
	PS_TOKEN,
	PS_STRING,
	PS_ESCAPE
};

enum ParseState stackedState;

/*
 * Parse user command line
 */
void parseargs(char *argstr, int *argc_p, char **argv, char** resid)
{
	int argc = 0;
	char c;
	enum ParseState lastState = PS_WHITESPACE;

	/* tokenize the argstr */
	while ((c = *argstr) != 0) {
		enum ParseState newState;

		if (c == ';' && lastState != PS_STRING && lastState != PS_ESCAPE)
			break;

		if (lastState == PS_ESCAPE) {
			newState = stackedState;
		} else if (lastState == PS_STRING) {
			if (c == '"') {
				newState = PS_WHITESPACE;
				*argstr = 0;
			} else {
				newState = PS_STRING;
			}
		} else if ((c == ' ') || (c == '\t')) {
			/* whitespace character */
			*argstr = 0;
			newState = PS_WHITESPACE;
		} else if (c == '"') {
			newState = PS_STRING;
			*argstr++ = 0;
			argv[argc++] = argstr;
		} else if (c == '\\') {
			stackedState = lastState;
			newState = PS_ESCAPE;
		} else {
			/* token */
			if (lastState == PS_WHITESPACE) {
				argv[argc++] = argstr;
			}
			newState = PS_TOKEN;
		}

		lastState = newState;
		argstr++;
	}
	
	argv[argc] = NULL;
	if (argc_p != NULL)
		*argc_p = argc;

	if (*argstr == ';') {
		*argstr++ = '\0';
	}
	*resid = argstr;
}

/* add user command */
void add_command(user_command_t *cmd)
{
	if (head_cmd == NULL) {
		head_cmd = tail_cmd = cmd;
	} else {
		tail_cmd->next_cmd = cmd;
		tail_cmd = cmd;
		tail_cmd->next_cmd = NULL; /*added by FCY ,2007-11-03*/

	}
	/*printf("Registered '%s' command\n\r", cmd->name);*/
}

/* find command */
user_command_t *find_cmd(const char *cmdname)
{
	user_command_t *curr;

	/* do a string compare for the first offset character of cmdstr
	  against each number of the cmdlist */
	curr = head_cmd;
	while(curr != NULL) {
		if (strncmp(curr->name, cmdname, strlen(cmdname)) == 0)
			return curr;
		curr = curr->next_cmd;
	}
	return NULL;
}

/* execute a function */
void execcmd(int argc, const char **argv)
{
	user_command_t *cmd = find_cmd(argv[0]);

	if (cmd == NULL) {
		printf("Could not found '%s' command\n\r", argv[0]);
		printf("If you want to know available commands, type 'help'\n\r");
		return;
	}
	/*printf("execcmd: cmd=%s, argc=%d\n\r", argv[0], argc);*/

	cmd->cmdfunc(argc, argv);
}

/* parse and execute a string */
void exec_string(char *buf)
{
	int argc;
	char *argv[16];
	char *resid;

	while (*buf) {
		memset(argv, 0, sizeof(argv));
		parseargs(buf, &argc, argv, &resid);
		if (argc > 0)
			execcmd(argc, (const char **)argv);
		buf = resid;
	}
}

/* help command */
void command_help(int argc, const char **argv)
{
	user_command_t *curr;

	/* help <command>. invoke <command> with 'help' as an argument */
	if (argc == 2) {
		if (strncmp(argv[1], "help", strlen(argv[1])) == 0) {
			printf("   Are you Playboy?\n\r");
			return;
		}
		argv[0] = argv[1];
		argv[1] = "help";
		execcmd(argc, argv);
		return;
	}

	printf("Usage:\n\r");
	curr = head_cmd;
	while(curr != NULL) {
		printf("   %s\n\r", curr->helpstr);
		curr = curr->next_cmd;
	}
}
user_command_t help_cmd = {
	"help",
	command_help,
	NULL,
	"help  [{cmds}] \t\t\t-- Help  about help?"
};

/* boot command */
void command_boot(int argc, const char *argv[])
{
    if(argc >= 1)
    {
    	if((strncmp(argv[1], "help", strlen(argv[1])) == 0) && (argc == 2))
    	{	
			printf("Usage:\n\r   boot\n\r");return;
    	}
	    printf("Reset all PLD and reboot discovery now ...\n\r\n\r");

	    GPIO_ResetBits(CRESET1_B_PORT, CRESET1_B_PIN);
	    GPIO_ResetBits(CRESET2_B_PORT, CRESET2_B_PIN);
		Delay(100);

		GPIO_SetBits(CRESET1_B_PORT, CRESET1_B_PIN);
		GPIO_SetBits(CRESET2_B_PORT, CRESET2_B_PIN);

	    NVIC_SystemReset();
    }
}
user_command_t boot_cmd = {
	"boot",
	command_boot,
	NULL,
	"boot  {argv[]} \t\t\t-- Reset all PLD and reboot the discovery"
};

/* go command */
void command_go(int argc, const char *argv[])
{   
    int address;
    if(argc >= 1)
    {
    	if((strncmp(argv[1], "help", strlen(argv[1])) == 0) && (argc == 2))
    	{	
			printf("Usage:\n\r   go [addr]\n\r");return;
    	}
        else
        if(argc == 2)
        {
            address=atox(argv[1]);
        }
        else
        {
            address=0x08000000;
        }
        printf("Run the Application on 0x%08x now ...\n\r\n\r",address);
        run_app(address);
    }
}
user_command_t go_cmd = {
	"go",
	command_go,
	NULL,
	"go    {argv[]} \t\t\t-- Run   the Application on address"
};

/* disp command */
void command_disp(int argc, const char *argv[])
{   
    unsigned int i,j,address,length;
    if(argc >= 1)
    {
    	if((strncmp(argv[1], "help", strlen(argv[1])) == 0) && (argc == 2))
    	{	
			printf("Usage:\n\r   disp [addr] [length]\n\r");return;
    	}
        else
        if(argc == 2)
        {
            address=atox(argv[1]);
            length=0x100;
        }
        else
        if(argc == 3)
        {
            address=atox(argv[1]);
            length=atox(argv[2]);
        }
        else
        {
            address=0x08004000;length=0x100;
        }
        printf("Display the Data from 0x%08x to 0x%08x ...\n\r\n\r",address,address+length);
        //
        for(i=address;i<address+length;)
        {
            printf("\n\r%08xh:",i-address);
            for(j=0;j<16;j++)
            {printf(" %02x",(unsigned char)(*(__IO uint8_t*)(i++)));}
        }
        printf("\n\r");
    }
}
user_command_t disp_cmd = {
	"disp",
	command_disp,
	NULL,
	"disp  {argv[]} \t\t\t-- Disp  the Data from address"
};

/* load command flash lattice thru SPI1*/
void command_pld1_flash(int argc, const char *argv[])
{
    unsigned int address, length;
    int count = 0;
    if(argc >= 1)
    {
    	if((strncmp(argv[1], "help", strlen(argv[1])) == 0) && (argc == 2))
    	{
			printf("Usage:\n\r   pld1_load\n\r");return;
    	}
        else
        if(argc == 2)
        {
            address=atox(argv[1]);
        }
        else
        {
        	address=&store[128];
			length=length_store;
        }
        printf("Download the binary 0x%04x from 0x%08x ...\n\r\n\r", length, address);

        u16 i = 0;
    	while (!( ((GPIO_ReadInputData(CDONE1_PORT)) & CDONE1_PIN)) && (i<10))
    	{
    		lattice_one_io_init();
			lattice_one_load(address, length);
			printf("\r%d", i++);
			Delay(100);
    	}
    	if ( (GPIO_ReadInputData(CDONE1_PORT)) & CDONE1_PIN)
		{	printf("\rPLD1 loaded\n", 0x0);
		} else
		{	printf("\rPLD1 load failed\n", 0x0);
		}
    }
}
user_command_t pld1load_cmd = {
	"pld1_load",
	command_pld1_flash,
	NULL,
	"pld1_load   \t\t\t\t-- Load the binary to PLD1"
};

/* load command flash lattice thru SPI1*/
void command_pld2_flash(int argc, const char *argv[])
{
    unsigned int address, length;
    int count = 0;
    if(argc >= 1)
    {
    	if((strncmp(argv[1], "help", strlen(argv[1])) == 0) && (argc == 2))
    	{
			printf("Usage:\n\r   pld2_load\n\r");return;
    	}
        else
        if(argc == 2)
        {
            address=atox(argv[1]);
        }
        else
        {
        	address=&store[128];
			length=length_store;
        }
        printf("Download the binary 0x%04x from 0x%08x ...\n\r\n\r", length, address);

        u16 i = 0;
    	while (!( ((GPIO_ReadInputData(CDONE2_PORT)) & CDONE2_PIN)) && (i<10))
    	{
    		lattice_two_io_init();
			lattice_two_load(address, length);
			printf("\r%d", i++);
			Delay(100);
    	}
    	if ( (GPIO_ReadInputData(CDONE2_PORT)) & CDONE2_PIN)
    	{	printf("\rPLD2 loaded\n", 0x0);
    	} else
    	{	printf("\rPLD2 load failed\n", 0x0);
    	}
    }
}
user_command_t pld2load_cmd = {
	"pld2_load",
	command_pld2_flash,
	NULL,
	"pld2_load   \t\t\t\t-- Load the binary to PLd2"
};
/* disp command from ram store*/
void command_dispfram(int argc, const char *argv[])
{
    unsigned int c,i,j,address,length;

    if(argc >= 1)
    {
    	if((strncmp(argv[1], "help", strlen(argv[1])) == 0) && (argc == 2))
    	{
			printf("Usage:\n\r   disp [addr] [length]\n\r");return;
    	}
        else
        if(argc == 2)
        {
            address=atox(argv[1]);
            length=0x100;
        }
        else
        if(argc == 3)
        {
            address=atox(argv[1]);
            length=atox(argv[2]);
        }
        else
        {
//        	address=0x20005000;
            address=&store[128];
            length=length_store;
		//PACKET_SIZE * packetNumber;
        }
        printf("Display the Data of fram from 0x%08x to 0x%08x ...\n\r\n\r",address,address+length);
        //
        for(i=address;i<address+length;)
        {
        	if (!(i%16))
        		{printf("\n\r%08xh:",i-address);}
            printf(" %02x",(unsigned char)(*(__IO uint8_t*)(i++)));
        }
        printf("\n\r");

    }
}
user_command_t dfram_cmd = {
	"dfram",
	command_dispfram,
	NULL,
	"dfram  {argv[]} \t\t\t-- Disp  the Data from address"
};

/*load binary to ram store*/
void ram_load(int argc, const char *argv[])
{
    unsigned int address;
    int i, count;
    puart_buf = 0x20000000;
    if(argc >= 1)
    {
    	if((strncmp(argv[1], "help", strlen(argv[1])) == 0) && (argc == 2))
    	{
			printf("Usage:\n\r   ram_load [addr]\n\r");return;
    	}
        else
        if(argc == 2)
        {
            address=atox(argv[1]);
        }
        else
        {
//        	address=0x20005000;
        	address=&store[0];
        }
        printf("Download the binary to ram now ...\n\r\n\r");
        USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
        xmodem_rx(address);
        length_store = (store[121] << 8) + store[120];
//        USART_ITConfig(USART3,USART_IT_RXNE,DISABLE);
        printf("\n\r", 0);
#ifdef TRACEUART
        for (count=0; count<length_store; count++)
        {
        	if (!(count%16))
			  {traceprintf("\n");}
        	traceprintf("%d:%x\t", count, *(store+count));
        }
#endif //TRACEUART

    }
}
user_command_t ramload_cmd = {
	"ramload",
	ram_load,
	NULL,
	"ramload  {argv[]} \t\t\t-- Load binary to ram"
};

/*wave generator*/
void wave_start(int argc, const char *argv[])
{
    unsigned int second;
    int i, count;
    puart_buf = 0x20000000;
    if(argc >= 1)
    {
    	if((strncmp(argv[1], "help", strlen(argv[1])) == 0) && (argc == 2))
    	{
			printf("Usage:\n\r   wave_play [sec]\n\r");return;
    	}
        else
        if(argc == 2)
        {
            second=atox(argv[1]);
        }
        else
        {
        	second=3;
        }
        printf("Generate sinus waveform now ...\n\r");
//        WavePlayerInit(AUDIOFREQ);
        GPIO_SetBits(BLINK_PORT, (0xF << LED1));
        WavePlayBack(AUDIOFREQ, second);
        GPIO_ResetBits(BLINK_PORT, (0xF << LED1));
    }
}
user_command_t wave_start_cmd = {
	"wave",
	wave_start,
	NULL,
	"wave_play {argv[]} \t\t\t-- Generate sinus waveform"
};

/*i2c checker*/
void i2c_check(int argc, const char *argv[])
{
    unsigned int reg, second;
    int i, count;
//    puart_buf = 0x20000000;
    if(argc >= 1)
    {
    	if((strncmp(argv[1], "help", strlen(argv[1])) == 0) && (argc == 2))
    	{
			printf("Usage:\n\r   i2c [reg][sec]\n\r");return;
    	}
        else
        if(argc == 2)
        {
            second=atox(argv[1]);
            reg = 0x00;
        }
        else
        {
        	second	= atox(argv[2]);
        	reg		= atox(argv[1]);
        }
        printf("Try read I2C register of codec ...\n\r");
        while(second)
		{
        	i = 50;
        	while(i)
        	{
        		Codec_ReadRegister(reg);
        		i--;
        		Delay(20);
        	}
        	second--;
		}
    }
}
user_command_t i2c_check_cmd = {
	"i2c",
	i2c_check,
	NULL,
	"i2c check {argv[]} \t\t\t-- I2C read register from codec"
};
/* Register basic user commands */
int init_builtin_cmds(void)
{
	add_command(&help_cmd);
	add_command(&boot_cmd);
	add_command(&ramload_cmd);
//    add_command(&disp_cmd);
//    add_command(&dfram_cmd);
    add_command(&pld1load_cmd);
    add_command(&pld2load_cmd);
//    add_command(&wave_start_cmd);
    add_command(&i2c_check_cmd);
//    add_command(&go_cmd);
   
	return 0;
}
