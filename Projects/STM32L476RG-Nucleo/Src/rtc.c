/**
  ******************************************************************************
  * File Name          : RTC.c
  * Description        : This file provides code for the configuration
  *                      of the RTC instances.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "rtc.h"

/* USER CODE BEGIN 0 */
RTC_AlarmTypeDef sAlarmA;
RTC_AlarmTypeDef sAlarmB;
RTC_TimeTypeDef RTC_TimeStructe;
RTC_DateTypeDef RTC_DateStructe;
/* USER CODE END 0 */

//RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Alarm_Init(void)
{
    /**Enable the Alarm A 
    */
  sAlarmA.AlarmTime.Hours = Set_Time.Hours;//0x0;
  sAlarmA.AlarmTime.Minutes = Set_Time.Minutes;//0x0;
  sAlarmA.AlarmTime.Seconds = Set_Time.Seconds;//0x10;
  sAlarmA.AlarmTime.SubSeconds = 0x0;
  sAlarmA.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarmA.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarmA.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
  sAlarmA.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarmA.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarmA.AlarmDateWeekDay = 0x1;
  sAlarmA.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&RtcHandle, &sAlarmA, RTC_FORMAT_BCD) != HAL_OK)
  {
    //_Error_Handler(__FILE__, __LINE__);
  }

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */
    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();

    /* RTC interrupt Init */
    HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0, 2);
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
  /* USER CODE BEGIN RTC_MspInit 1 */

  /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();

    /* RTC interrupt Deinit */
    HAL_NVIC_DisableIRQ(RTC_Alarm_IRQn);
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	AlarmA_Flag=1;
}
void HAL_RTCEx_AlarmBEventCallback(RTC_HandleTypeDef *hrtc)
{
  AlarmB_Flag=1;
}
uint8_t RTC_Alarm(Alarm_TimeTypeDef start_time,uint16_t period)  //设置闹钟B作为起始时间，同时将间隔S分离
{
	sAlarmB.AlarmTime.Hours = start_time.Hours;//0x0;            //闹钟B的起始时间设置，在该时刻进入进入闹钟B的中断，触发闹钟A闪烁第一次并从此时刻开始计时
	sAlarmB.AlarmTime.Minutes = start_time.Minutes;//0x0;
	sAlarmB.AlarmTime.Seconds = start_time.Seconds;//0x10;
	sAlarmB.AlarmTime.SubSeconds = 0x0;
	sAlarmB.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sAlarmB.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
	sAlarmB.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
	sAlarmB.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;//RTC_ALARMSUBSECONDMASK_ALL;
	sAlarmB.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
	sAlarmB.AlarmDateWeekDay = 0x1;
	sAlarmB.Alarm = RTC_ALARM_B;
	if (HAL_RTC_SetAlarm_IT(&RtcHandle, &sAlarmB, RTC_FORMAT_BIN) != HAL_OK)
	{
		return 0;
		//_Error_Handler(__FILE__, __LINE__);
	}
	AlarmB_Time.Seconds = period%60;
	AlarmB_Time.Minutes = period/60%60;
	AlarmB_Time.Hours = period/60/60;
	return 1;
}
void AlarmA_Interval_Set(void)
{
	u32 Start_Time_S,Set_Alarm_Time_S;
	if(Alarm_Num == 0)
	{		
//		Set_Alarm_Time_S=(sAlarmB.AlarmTime.Hours*3600)+(sAlarmB.AlarmTime.Minutes*60)+sAlarmB.AlarmTime.Seconds;
//		Alarm_Num = 1;
	}
	else if(Alarm_Num == 1)
	{
	 Set_Alarm_Time_S=(sAlarmA.AlarmTime.Hours*3600)+(sAlarmA.AlarmTime.Minutes*60)+sAlarmA.AlarmTime.Seconds;
	}

	Set_Alarm_Time_S +=LED_Period; // ?? ?? ???

	if(Set_Alarm_Time_S>=86400)     // 24*60*60=86400 ?? 24 ??
	Set_Alarm_Time_S-=86400;
		
	Set_Alarm_Compare = Set_Alarm_Time_S;
	
	sAlarmA.AlarmTime.Hours = Set_Alarm_Time_S/3600;
	sAlarmA.AlarmTime.Minutes = Set_Alarm_Time_S%3600/60;
	sAlarmA.AlarmTime.Seconds = Set_Alarm_Time_S%60;
	sAlarmA.AlarmTime.SubSeconds = 0x0;
	sAlarmA.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sAlarmA.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
	sAlarmA.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
	sAlarmA.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;//RTC_ALARMSUBSECONDMASK_ALL;
	sAlarmA.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
	sAlarmA.AlarmDateWeekDay = 0x1;
	sAlarmA.Alarm = RTC_ALARM_A;
	if (HAL_RTC_SetAlarm_IT(&RtcHandle, &sAlarmA, RTC_FORMAT_BIN) != HAL_OK)
	{
		//_Error_Handler(__FILE__, __LINE__);
	}
	
}


void get_Set_Alarm_Time(void)  
{

	u32 Start_Time_S,Set_Alarm_Time_S,Now_Time_S;
		
	u32 i;
	Start_Time.Hours = start_time[0];//system_param.start_time[0];//获取flash中的起始时间与闪烁间隔时间
	Start_Time.Minutes = 	start_time[1];//system_param.start_time[1];
	Start_Time.Seconds = start_time[2];//system_param.start_time[2];
//	LED_Period = (system_param.led_period_interval[0]<<16) + (system_param.led_period_interval[1]<<8) + system_param.led_period_interval[2];
	
	Start_Time_S=(Start_Time.Hours*3600)+(Start_Time.Minutes*60)+Start_Time.Seconds; // Start_Time ????
		
	/* Get the RTC current Time */
	HAL_RTC_GetTime(&RtcHandle, &RTC_TimeStructe, RTC_FORMAT_BIN);
	/* Get the RTC current Date */
	HAL_RTC_GetDate(&RtcHandle, &RTC_DateStructe, RTC_FORMAT_BIN);
		
	Now_Time_S=(RTC_TimeStructe.Hours*3600)+(RTC_TimeStructe.Minutes*60)+RTC_TimeStructe.Seconds;// 装换为s
		
	if(Now_Time_S>=Start_Time_S)	// ???? ?? ????
	{
		i=0;
		while(1)
		{
		 i++;
		 if(i>18000) break; // ?????  ???? ?? ????
		 Start_Time_S+=LED_Period;
		 if(Start_Time_S>Now_Time_S)break;
		}
		Set_Alarm_Time_S=Start_Time_S;
	}

	else //?????? ????
	{
		i=0;
		while(1)
		{
		 i++;
		 if(i>18000) break; // ?????  ???? ?? ???? ?????
			
			if(Start_Time_S>=LED_Period)
			Start_Time_S-=LED_Period;
		 if(Start_Time_S<=Now_Time_S)break;
		}
		Set_Alarm_Time_S=Start_Time_S+LED_Period;
	}

	if(Set_Alarm_Time_S>=86400)     // 24*60*60=86400 ?? 24 ??
	 Set_Alarm_Time_S-=86400;
		
  Set_Alarm_Compare = Set_Alarm_Time_S;
	
	sAlarmA.AlarmTime.Hours = Set_Alarm_Time_S/3600;
  sAlarmA.AlarmTime.Minutes = Set_Alarm_Time_S%3600/60;
  sAlarmA.AlarmTime.Seconds = Set_Alarm_Time_S%60;
	sAlarmA.AlarmTime.SubSeconds = 0;
  sAlarmA.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarmA.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarmA.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
  sAlarmA.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;//RTC_ALARMSUBSECONDMASK_ALL;
  sAlarmA.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
  sAlarmA.AlarmDateWeekDay = 0x1;
	sAlarmA.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&RtcHandle, &sAlarmA, RTC_FORMAT_BIN) != HAL_OK)
  {
    //_Error_Handler(__FILE__, __LINE__);
  }
	Alarm_Num = 1;
}

void get_Set_Alarm_Time_AlarmB(void)  //闹钟B为10s关掉lora后的1.5s定时
{

	u32 Set_AlarmB_Time_S,Now_TimeB_S;

	/* Get the RTC current Time */
	HAL_RTC_GetTime(&RtcHandle, &RTC_TimeStructe, RTC_FORMAT_BIN);
	/* Get the RTC current Date */
	HAL_RTC_GetDate(&RtcHandle, &RTC_DateStructe, RTC_FORMAT_BIN);
		
	Now_TimeB_S=(RTC_TimeStructe.Hours*3600)+(RTC_TimeStructe.Minutes*60)+RTC_TimeStructe.Seconds;// 装换为s
	Set_AlarmB_Time_S = 	Now_TimeB_S + 2;

	if(Set_AlarmB_Time_S>=86400)     // 24*60*60=86400 ?? 24 ??
	 Set_AlarmB_Time_S-=86400;
		
  //if(Set_Alarm_Compare != Set_AlarmB_Time_S)
	{
		sAlarmB.AlarmTime.Hours = Set_AlarmB_Time_S/3600;
		sAlarmB.AlarmTime.Minutes = Set_AlarmB_Time_S%3600/60;
		sAlarmB.AlarmTime.Seconds = Set_AlarmB_Time_S%60;
		sAlarmB.AlarmTime.SubSeconds = 0;//500
		sAlarmB.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
		sAlarmB.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
		sAlarmB.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
		sAlarmB.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;//RTC_ALARMSUBSECONDMASK_ALL;
		sAlarmB.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
		sAlarmB.AlarmDateWeekDay = 0x1;
		sAlarmB.Alarm = RTC_ALARM_B;
		if (HAL_RTC_SetAlarm_IT(&RtcHandle, &sAlarmB, RTC_FORMAT_BIN) != HAL_OK)
		{
			//_Error_Handler(__FILE__, __LINE__);
		}
  }
}



/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
