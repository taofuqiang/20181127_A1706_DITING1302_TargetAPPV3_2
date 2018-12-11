/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          app_board.h
*\Description   
*\Note          
*\Log           2018.01.19    Ver 1.0    Job
*               创建文件。
***************************************************************************************************/
#ifndef _APP_BOARD_H
#define _APP_BOARD_H
#include "public_type.h"
#include "stm32_uart.h"
#include "stm32_spi.h"
#include "stm32_Inrtc.h"
#include "stm32_lptim.h"
#include "drv_sx1278.h"
#include "drv_watchdog.h"
#include "stm32_gpio_irq.h"
#include "lora_pp.h"
#include "iap_if.h"
#include "flash_if.h"
#include "low_power.h"
#include "ota_protocol.h"
#include "adc.h"
#include "stm32l4xx_ll_adc.h"
//#include "rtc.h"
//#include "stm32l4xx_hal_rtc.h"
//调试功能

//????,????????,????flash
typedef struct system_param_type_st
{
	u32 Power_First_Flag;
//	u8 softwave_version_marker;//软件版本
//	u8 hardwave_version_marker;//硬件版本
//	u8 decice_id;   //设备类型
	u8 RTC_WakeUp_Time;//RTC唤醒时间
	u8 adc_data_l;//adc电量数据高低位
	u8 adc_data_h;
	u8 start_time[3];//h m l 
  u8 led_period_interval[3];// h m l
	u8 latitude[8];// 度 分 秒
	u8 longitude[8];// 度 分 秒
	u8 maker_run_state;
	u16 src_addr;//lora  发送目标ID
	uint16_t Channel;//信道 
	u8 ROOM_TEMPERATURE_PPM;
}system_param_type;
/*Define a Counter Struct*/
typedef struct
{
  uint8_t Enable;                  /*!< Enable the timeout Counter*/
	uint32_t Counter;                 /*!< the Counter Register*/
	uint32_t Threshold;               /*!< the Threshold Register*/
	uint8_t Status;                  /*!< the Status Register*/	
	
}Timeout_TypeDef;
/////////////////gps init begin/////////////////////////
typedef struct{
    double latitude; //纬度
    double longitude; //经度
    uint8_t latitude_Degree;    //度
    uint8_t        latitude_Cent;   //分
    uint8_t     latitude_Second; //秒
    uint8_t longitude_Degree;    //度
    uint8_t        longitude_Cent;  //分
    uint8_t     longitude_Second; //秒
    float     speed; //速度
    float     direction; //航向
    float     height; //海拔高度
    int satellite;
    uint8_t     NS;
    uint8_t     EW;
    //DATE_TIME 	D;
		TIME        InRTC;//解析时间
		uint32_t    CountRTC;//时间戳，为了设置时间时更准确
		uint32_t    rtc_tick;//解析成功RTC的tick
}GPS_INFO;
typedef struct{
    uint8_t year; 
    uint8_t month; 
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
		uint8_t latitude_Degree;    //度
    uint8_t        latitude_Cent;   //分
    uint8_t     latitude_Second; //秒
    uint8_t longitude_Degree;    //度
    uint8_t        longitude_Cent;  //分
    uint8_t     longitude_Second; //秒
}GNRMC_INFO;

typedef struct{
    uint8_t state; 
    uint8_t count; 
}ShouShi_TypeDef;
////////////////////////end///////////////////////////////
//RTC
/** 
  * @brief  RTC Time structure definition  
  */
typedef struct
{
  uint8_t Hours;            /*!< Specifies the RTC Time Hour.
                                 This parameter must be a number between Min_Data = 0 and Max_Data = 12 if the RTC_HourFormat_12 is selected.
                                 This parameter must be a number between Min_Data = 0 and Max_Data = 23 if the RTC_HourFormat_24 is selected */

  uint8_t Minutes;          /*!< Specifies the RTC Time Minutes.
                                 This parameter must be a number between Min_Data = 0 and Max_Data = 59 */

  uint8_t Seconds;          /*!< Specifies the RTC Time Seconds.
                                 This parameter must be a number between Min_Data = 0 and Max_Data = 59 */

}Alarm_TimeTypeDef;

//#define USING_USART_PRINTF

#define LED1_ON            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET)
#define LED1_OFF           HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET)
#define LED2_ON            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET)
#define LED2_OFF           HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET)
#define LED_Run_DoubleON 			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1|GPIO_PIN_2, GPIO_PIN_RESET)
#define LED_Run_SingleON 			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET)
#define LED_Run_OFF 		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1|GPIO_PIN_2, GPIO_PIN_SET)	 
#define GPS_PowerON 		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET)
#define GPS_PowerOFF 		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET)
#define State_Lp_Run       				0     //low power run
#define State_Lora_IT     		 		1    //receive lora data outpf lp
#define State_GPS_Data_Read       2			//read gps data
#define State_GPS_Data_Handle     3   //uploading data or calibration time
#define State_LED_Debug           4

#define UART1_RxBuf_SIZE       125
#define ONE_DAY                 (60 * 60 * 24)     /*!<1日秒数*/
#define ONE_HOUR                (60 * 60)     /*!<1小时秒数*/
#define ONE_MINUTE              (60)     /*!<1分钟秒数*/
//RTC
#define Timing_Hours 0x00
#define Timing_Minutes 0x00
#define Timing_Seconds 0x15
#define Start_Hours 0x00
#define Start_Minutes 0x00
#define Start_Seconds 0x15
//ADC
#define vddval_offset 0
#define adc_value_offset 440
#define VCC_CHECK_CHANNEL ADC_CHANNEL_5
/* Init variable out of expected ADC conversion data range */
//RTC calibration
#define ROOM_TEMPERATURE 		25
//#define ROOM_TEMPERATURE_PPM 	58
#define MC306_G_06Q_32768
//#define MC_xx_32768
//#define MC_yy_32768
//#define MC_zz_32768


extern switch_type MSG_LED;
extern UartDevType UartTTL;
extern LoRa_Dev_type LoRa_Dev;

//全局变量
extern uint8_t softwave_version_marker;//软件版本
extern uint8_t hardwave_version_marker;//硬件版本
extern uint8_t decice_id;   //设备类型
extern uint8_t RTC_WakeUp_It;
extern uint32_t RTC_WakeUp_Period;
extern uint32_t RTC_SyncTime_Last;
extern uint32_t RTC_SyncTime_Current;
extern uint8_t Marker_State;
extern system_param_type system_param;//系统中不保存的参数   app参数，可以更改为保存
extern uint8_t  start_time[3];
extern uint16_t ADC_Value;
extern float VddVal;
extern uint16_t adc_data;
extern uint16_t u16VrefintCal;
extern uint16_t u16VrefintVal;
//extern uint16_t T_calibration_num;
extern uint16_t get_Set_Alarm_Time_num;
extern uint8_t UART1_RxBuf_GPS[UART1_RxBuf_SIZE];
extern uint8_t UART1_RxFlag_GPS;
extern uint8_t Read_GPS_Flag;
extern GNRMC_INFO GPS_GLOBLE_INFO;
extern uint8_t  led_flag;
extern uint32_t Now_Time_Start;
extern uint16_t Lora_Recive_Flag;
extern uint8_t Lora_Recive_Count;
//RTC
extern Alarm_TimeTypeDef Set_Time;
extern Alarm_TimeTypeDef AlarmA_Time;
extern Alarm_TimeTypeDef AlarmB_Time;
extern Alarm_TimeTypeDef Start_Time;
extern uint8_t AlarmA_Flag;
extern uint8_t AlarmB_Flag;
extern uint32_t Alarm_LED_Period;
extern uint32_t LED_Period;
extern uint8_t Alarm_Num;
extern uint8_t last_state;
extern uint8_t runtime_counter[4];
extern uint32_t run_time;
extern uint8_t ROOM_TEMPERATURE_PPM;
extern Timeout_TypeDef LED_Timeout;//LED超时60s
extern uint8_t  Lora_tens_flag;
extern uint32_t Set_Alarm_Compare;
extern uint16_t cal_num;
extern ShouShi_TypeDef  ShouShi;

void app_board_init(void);
void Led_Pin_Init(void);
void Gps_Pin_Init(void);
void LoRa_Set(void);
void LoRaRecv_CallBack(void* p, u16 src_addr, u8* buf, u16 len, s16 rssi, s8 snr);
void Lp_Run(void);
void Lora_DataAnalyse(void);
void App_Command(void);
void stm32_system_param_notsave_init(void);
uint32_t app_param_read(uint8_t* buf,uint8_t fuc_param, uint32_t len);
uint32_t device_param_read(uint8_t* buf,uint8_t fuc_param, uint32_t len);
uint32_t gps_param_read(uint8_t* buf,uint8_t fuc_param, uint32_t len);
void ADC_GetVol(uint32_t chn,uint16_t* rp,uint8_t num);
void ADC_ChnCfg(uint32_t chn);
HAL_StatusTypeDef HAL_RTC_SetSmoothCalib(RTC_HandleTypeDef* hrtc, uint32_t SmoothCalibValue);
void HAL_RTC_Calibration(void);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
bool GPS_RMC_Parse(GPS_INFO *GPS);
void Lora_SendGPS_lastData(void);
void app_param_save(void);
void app_param_load(void);
void app_param_default(void);
void It_Judge(void);
#endif /*_APP_BOARD_H*/

