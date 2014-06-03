/* ----- LED definitions --------------------------------------------------- */
/*--------------------------------------------------------*/
#define BLINK_PORT		GPIOD
#define LED4			15
#define LED3			14
#define LED2			13
#define LED1			12
#define BLINK_RCC_BIT   RCC_AHB1Periph_GPIOD
/*GPIO DEFS for LED control*/


/*--------------------------------------------------------*/
#define 	LATTICE1_RCC_BIT	RCC_AHB1Periph_GPIOB

#define 	LATTICE2_RCC_BIT	RCC_AHB1Periph_GPIOD

#define		CRESET1_B_PORT	GPIOB
#define		CRESET1_B_PIN	GPIO_Pin_0

#define		CDONE1_PORT		GPIOB
#define		CDONE1_PIN		GPIO_Pin_1

#define		CRESET2_B_PORT	GPIOD
#define		CRESET2_B_PIN	GPIO_Pin_1

#define		CDONE2_PORT		GPIOD
#define		CDONE2_PIN		GPIO_Pin_7

#define		SPI1_PORT		GPIOA
#define		SPI1_SS			GPIO_Pin_4
#define		SPI1_CLK		GPIO_Pin_5
#define		SPI1_MOSI		GPIO_Pin_7

#define		SPI2_PORT		GPIOB
#define		SPI2_SS			GPIO_Pin_9
#define		SPI2_CLK		GPIO_Pin_15
#define		SPI2_MOSI		GPIO_Pin_13

/*GPIO DEFS for SPI1 loader control*/
