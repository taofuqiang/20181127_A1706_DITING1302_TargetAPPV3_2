
/***************************************************************************************************
*                    (c) Copyright 1992-2018 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          sensor_gpio.c
*\Description   
*\Note          
*\Log           2018.05.02    Ver 1.0    �ӹ���
*               �����ļ���
***************************************************************************************************/
#include "sensor_gpio.h"

//Ϊ��ֹ����� 
//LIS3DSH
/*
INT2 PA1
INT1 PA2
CS PA3
SEL/SDO SPI1MISO PA6
SDA/SDO SPI1MOSI PA7
SCL SPI1SCK PA5
*/
void lis3dsh_interrupt_gpio_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
    __HAL_RCC_GPIOA_CLK_ENABLE();  
    GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
	
	
	GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
//	GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2;
//	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
//	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
//	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
//	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);	
}

//MSG PB4
/*
led��������10ma���
*/
void msg_gpio_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	__HAL_RCC_GPIOB_CLK_ENABLE();  
	/*Configure GPIO pins :PB4 MSG*/
	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); 
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);//ָʾ����
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);//ָʾ����
}

//AS62T30  20180411 B2 ����ʹ����Щ���ţ�����Ҫ����
/*
MD0 PA8  lis3dsh sw
MD1 PA12   can sw
AUX PC13
*/
void lora_as62t30_gpio_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	__HAL_RCC_GPIOA_CLK_ENABLE();  

	GPIO_InitStruct.Pin =  GPIO_PIN_8|GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
			
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);	//LIS3DSH��������
	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);	//LIS3DSH�ϵ�	
	
	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);	//CAN��������
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);	//CAN �ϵ�		
	
	__HAL_RCC_GPIOC_CLK_ENABLE();  
	/*Configure GPIO pin : PC13 */
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
}


//POWER PC0
void power_manage_gpio_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	__HAL_RCC_GPIOC_CLK_ENABLE();  
	/*Configure GPIO pins :PC0*/
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct); 
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);	//CPU��������
	//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);	//CPU�ϵ�
}

//PC3/PC2  PB10 ��Ƶ�ɼ� 20180411 B2 ��PC2��PC3�̽��������clk
void audio_gpio_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	__HAL_RCC_GPIOC_CLK_ENABLE();  
	GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	__HAL_RCC_GPIOB_CLK_ENABLE();  
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

//Ra 02 Module
/*
Rareset  PC12
DIO0 PC6
DIO1 PC7
DIO2 PC8 
DIO3 PC9
DIO4 PC11
DIO5 PC10
SPI2SCK  PB13
SPI2MISO PB14
SPI2MOSI PB15
SPI2CS   PB12
*/
//��ʼ����Ϊanalog ���������Ų�����
void ra02module_gpio_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	__HAL_RCC_GPIOC_CLK_ENABLE();  
	GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7 |GPIO_PIN_8|GPIO_PIN_9
						  |GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);	

	GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	
	
}
//UART3 PC4 PC5
void uart_debug_gpio_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	__HAL_RCC_GPIOC_CLK_ENABLE();  
	GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);		
}

//VCC CHECK PA0
void vcc_check_gpio_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	__HAL_RCC_GPIOA_CLK_ENABLE();  
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

//VCC SW PB11
/*SET��� RESET��3ma SETģʽ���ĵ�*/
void vcc_sw_gpio_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	__HAL_RCC_GPIOB_CLK_ENABLE();  
	
	GPIO_InitStruct.Pin = GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); 
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET);//		
	
}

//BD1612
/*
I2C1SDA  PB7
I2C1SCL  PB6
UART1RX  PA9
UART1TX  PA10
nRESET GPSRST PA15
1PPS GPSPPS PA11
GPSSW PB2
������PB2���� usart��������analogģʽ�� ���ĵ�
����25ma 3.3v
*/
void bd1612_gpio_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	__HAL_RCC_GPIOB_CLK_ENABLE();  
	
	GPIO_InitStruct.Pin = GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); 
	//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);//GPS����	
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);//GPS�ϵ�
	
	GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	
	//PA9,10usart
	__HAL_RCC_GPIOA_CLK_ENABLE();  	
	GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);	
	
	GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);	
}
//CANTX PB9
//CANRX PB8
/*
B0505S�Թ���Ӱ�첻����
*/
void can_gpio_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	__HAL_RCC_GPIOB_CLK_ENABLE();  
	GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
        * Free pins are configured automatically as Analog (this feature is enabled through 
        * the Code Generation settings)
*/
 void sensor_gpio_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	/*Configure GPIO pins : PA0 PA4 PA11 PA15 */
	GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : PB0 PB1   
						       
						    PB4 PB5  
						      */
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|
						  GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pins :  PC1 */
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin : PD2 */
	GPIO_InitStruct.Pin = GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	//AO3407�͵�ƽΪͨ �ߵ�ƽΪ��ͨ
	//lis3dsh_interrupt_gpio_init();	
//	msg_gpio_init();
	//lora_as62t30_gpio_init();
	power_manage_gpio_init();
	uart_debug_gpio_init();
	//audio_gpio_init();
	ra02module_gpio_init();
	vcc_check_gpio_init();
	//vcc_sw_gpio_init();
	bd1612_gpio_init();
	//can_gpio_init();
}


/***************************************************************************************************
******************************************** END OF FILE *******************************************
***************************************************************************************************/















