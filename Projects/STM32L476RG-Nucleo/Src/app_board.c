/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          app_board.c
*\Description   
*\Note          
*\Log           2018.01.19    Ver 1.0    Job
*               �����ļ���
***************************************************************************************************/
#include "app_board.h"
#include "rtc.h"
//LED
switch_type MSG_LED = {GPIOA, GPIO_PIN_2, FALSE};

//����
static GpioType UartTx = {GPIOA, GPIO_PIN_9, GPIO_AF7_USART1};
static GpioType UartRx = {GPIOA, GPIO_PIN_10, GPIO_AF7_USART1};
static u8 UartRxBuf[1024];

UartDevType UartTTL = {USART1, &UartTx, &UartRx, NULL, 9600, UartRxBuf, sizeof(UartRxBuf)};
//LoRa
static switch_type LoRaPW = {GPIOA, GPIO_PIN_4, FALSE};
static switch_type ResetPin = {GPIOC, GPIO_PIN_12, FALSE};
static GpioType  DIO0 = {GPIOC, GPIO_PIN_6};

static switch_type SPI2_CS = {GPIOB, GPIO_PIN_12, FALSE};
static GpioType SPI2_CLK = {GPIOB, GPIO_PIN_13, GPIO_AF5_SPI2};
static GpioType SPI2_MOSI = {GPIOB, GPIO_PIN_15, GPIO_AF5_SPI2};
static GpioType SPI2_MISO = {GPIOB, GPIO_PIN_14, GPIO_AF5_SPI2};

static SpiDevType SPI2_Dev = {SPI2, &SPI2_CS, &SPI2_CLK, &SPI2_MOSI, &SPI2_MISO, \
                        10*1000*1000, SPI_DATASIZE_8BIT, SPI_POLARITY_LOW, SPI_PHASE_1EDGE, SPI_FIRSTBIT_MSB};

LoRa_Dev_type LoRa_Dev = {&LoRaPW, &ResetPin, &SPI2_Dev, &DIO0, 475*1000*1000, 20};

//ȫ�ֱ���
LoRa_PP_type LoRa_PP;

uint8_t softwave_version_marker=0;//����汾
uint8_t hardwave_version_marker=0;//Ӳ���汾
uint8_t decice_id=0;   //�豸����
uint8_t RTC_WakeUp_It=0;
uint32_t RTC_WakeUp_Period=12;//
system_param_type system_param;//ϵͳ�в�����Ĳ���
uint8_t  start_time[3];
uint16_t ADC_Value;
uint16_t adc_data;
uint16_t u16VrefintCal=0;
uint16_t u16VrefintVal=0;
float VddVal;
//uint16_t T_calibration_num=0;
uint16_t get_Set_Alarm_Time_num=0;
uint8_t  led_flag=0;
enum
{
    Start = 0,
    G,
    P,
    R,
    M,
    C,
    Data,
    Check0,
    Check1,
};
uint8_t Read_GPS_Flag=0;
static GNRMC_INFO GPS_GLOBLE_INFO;
Timeout_TypeDef GPS_Timeout = {0,0,25,0};//GPS��ʱ300s,12*25=300
Timeout_TypeDef LED_Timeout = {0,0,120000,0};//LED��ʱ60s
//RTC
Alarm_TimeTypeDef Set_Time={Timing_Hours,Timing_Minutes,Timing_Seconds};//����A��һ�ν��ж�ʱ��
Alarm_TimeTypeDef Start_Time={Start_Hours,Start_Minutes,Start_Seconds};//LED��ʼʱ�̣�����B��
Alarm_TimeTypeDef AlarmA_Time={0x00,0x00,0x00};//���ʱ�䣨����A��
Alarm_TimeTypeDef AlarmB_Time={0x00,0x00,0x00};//���ʱ�䣨����B��
uint8_t AlarmA_Flag=0;
uint8_t AlarmB_Flag=0;
uint32_t Alarm_LED_Period=15;
uint32_t LED_Period=0;
uint8_t Alarm_Num=1;
uint8_t last_state=0;
uint8_t runtime_counter[4]={0x00,0x00,0x00,0x00};
uint32_t run_time=0;
uint8_t ROOM_TEMPERATURE_PPM = 82;
uint32_t Now_Time_Start=0;
uint16_t Lora_Recive_Flag=0;
uint8_t  Lora_Recive_Count=0;
uint8_t  Lora_tens_flag=0;
uint32_t Set_Alarm_Compare=0;
uint16_t cal_num = 0;
ShouShi_TypeDef  ShouShi={0,0};
uint8_t  lora_time_start = 1;
uint32_t RTC_SyncTime_Last=2;
uint32_t RTC_SyncTime_Current=2;
/***************************************************************************************************
*\Function      stm32_board_lowpower_cfg
*\Description   �͹�������
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.05.25    Ver 1.0    Job               
				����������
***************************************************************************************************/
void stm32_board_lowpower_cfg(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    //������������ ������
    //PA13 PA14  PB3 SWO
    //PH0 PH1 HSE ��ʱδʹ�� ��������
    //PC14 PC5 LSE

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();

    //GPIOA
    GPIO_InitStruct.Pin   = GPIO_PIN_All;
    //����
    GPIO_InitStruct.Pin &= (~(GPIO_PIN_13|GPIO_PIN_14));

    GPIO_InitStruct.Mode  = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 

    //GPIOB
    GPIO_InitStruct.Pin   = GPIO_PIN_All;
    //����
    GPIO_InitStruct.Pin &= (~(GPIO_PIN_3));

    GPIO_InitStruct.Mode  = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); 

    //GPIOC
    GPIO_InitStruct.Pin   = GPIO_PIN_All;
    //����
    GPIO_InitStruct.Pin &= (~(GPIO_PIN_14|GPIO_PIN_15));

    GPIO_InitStruct.Mode  = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct); 

    //GPIOD
    GPIO_InitStruct.Pin   = GPIO_PIN_All;
    GPIO_InitStruct.Mode  = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct); 

    //GPIOE
    GPIO_InitStruct.Pin   = GPIO_PIN_All;
    GPIO_InitStruct.Mode  = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct); 

    //GPIOF
    GPIO_InitStruct.Pin   = GPIO_PIN_All;
    GPIO_InitStruct.Mode  = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct); 

    //GPIOG
    GPIO_InitStruct.Pin   = GPIO_PIN_All;
    GPIO_InitStruct.Mode  = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct); 

    //GPIOH
    GPIO_InitStruct.Pin   = GPIO_PIN_All;
    GPIO_InitStruct.Mode  = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct); 

    __HAL_RCC_GPIOA_CLK_DISABLE();
    __HAL_RCC_GPIOB_CLK_DISABLE();
    __HAL_RCC_GPIOC_CLK_DISABLE();
    __HAL_RCC_GPIOD_CLK_DISABLE();
    __HAL_RCC_GPIOE_CLK_DISABLE();
    __HAL_RCC_GPIOF_CLK_DISABLE();
    __HAL_RCC_GPIOG_CLK_DISABLE();
    __HAL_RCC_GPIOH_CLK_DISABLE();
}
/***************************************************************************************************
*\Function      acs_app_board_init
*\Description   
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.01.19    Ver 1.0    Job               
				����������
***************************************************************************************************/
extern  void sensor_gpio_Init(void);
void app_board_init(void)
{
    //ȫ����������һ��
    stm32_board_lowpower_cfg();
	//sensor_gpio_Init();//
    //���Ź��ȳ�ʼ��
    watchdog_init();
    //���� ���ʹ�øô��ڵ��� �����ȳ�ʼ��
//    stm32_uart_init(&UartTTL);

    stm32_switch_init(&MSG_LED);

    GetRestFlag();
    //RTC
    stm32_Inrtc_init(TRUE);   
	 //LED
    Led_Pin_Init();
	//GPS
	Gps_Pin_Init();
	//ADC
	MX_ADC1_Init();
	//LORA SET
	LoRa_Set();
	printf("System Init...\n\r");
}
/* Private function prototypes -----------------------------------------------*/
#ifdef USING_USART_PRINTF
int fputc(int ch, FILE *f)
{
    stm32_uart_send(&UartTTL, (u8*)&ch, 1);
    return ch;
}
#endif /* __GNUC__ */
//led init
void Led_Pin_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
 
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1|GPIO_PIN_2, GPIO_PIN_SET);

  /*Configure GPIO pins : PA1 PA2 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}
//gps init
void Gps_Pin_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
 
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET); //�ر�GPS

  /*Configure GPIO pins : PA1 PA2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

void LoRaRecv_CallBack(void* p, u16 src_addr, u8* buf, u16 len, s16 rssi, s8 snr)
{
	iap_param.src_addr=src_addr;
	iap_param.lora_rssi=rssi;//
	iap_param.lora_snr=snr;//	
		
	memcpy(OTACommBuf, buf, len);
	OTACommBufLen = len;
	Marker_State=State_Lora_IT;//change run state for analyse lora data
	Lora_tens_flag = 1;
	Lora_Recive_Flag = 1;
	AlarmB_Flag=0;
}
//lora init
void LoRa_Set(void)
{   
	LoRa_Init(&LoRa_PP, &LoRa_Dev, iap_param.Channel*1000*1000, iap_param.ID, 60 * 1000, FALSE, stm32_lowpower_exit,300);
	LoRa_Recv_CallBack_Set(&LoRa_PP, LoRaRecv_CallBack, NULL);  
	LoRa_Start(&LoRa_PP);
}


uint8_t Marker_State = State_GPS_Data_Read;//State_Lp_Run;////�������״̬�ж�
void App_Command(void)
{
	switch(Marker_State)
	{
		case State_Lp_Run://in low power mode run 
		{
			Watchdog_Refresh(); //ÿ12sι��
			if(Read_GPS_Flag == 1)
			{
			  Marker_State=State_GPS_Data_Handle;//�����״̬����ֹ��GPS��ȡ����ʱƵ������͹��ģ�
				break;
			}
		#ifdef Target_APP
			Lp_Run();	
		#endif
			It_Judge();
			last_state = State_Lp_Run;
		}
		break;
		case State_Lora_IT: //init spi2,start receive lora data
		{	
			//Lora_Recive_Flag = 1;
			Watchdog_Refresh(); //��lora�����ڼ����ι������
			Lora_Recive_Count = 0;
			Lora_DataAnalyse();
		}
		break;
		case State_GPS_Data_Read:  //init usart1,start receive gps data
		{
//			Lora_SendGPS_lastData();
			stm32_uart_init(&UartTTL);//�򿪴���  ��ʼ��
			Read_GPS_Flag = 1;
			GPS_PowerON;
			HAL_NVIC_EnableIRQ(USART1_IRQn);
			GPS_Timeout.Enable=1;
			Lora_tens_flag =1;
			Marker_State=State_GPS_Data_Handle;//�����״̬����ֹGPS���¿���
		}
		break;
		case State_GPS_Data_Handle://received gps data,and handle data (calibration time/read location updata )
		{
			Watchdog_Refresh(); //ÿ12sι��
			It_Judge();
			last_state = State_GPS_Data_Handle;
		}
		break;
		case State_LED_Debug:// led debug mode
		{
			Watchdog_Refresh(); //ÿ12sι��
			It_Judge();
			last_state = State_LED_Debug;
		}
		break;
		default:break;
	}
}
//run low power
void Lp_Run(void)
{		
		stm32_lowpower_enter();
		stm32_lowpower_exit();
}
//lora data analyse
void Lora_DataAnalyse(void)
{
	OTA_Periodic_Handle();
}

void stm32_system_param_notsave_init(void)
{
	softwave_version_marker=0x03;
	hardwave_version_marker=0x02;
	decice_id=0x03;
}
uint32_t device_param_read(uint8_t* buf,uint8_t fuc_param, uint32_t len)
{
 u32 data_len;
	int i=0;
	//��ѯ��ǰID �ŵ��Ƿ��flash�浵��һ�£�һ����ʼͨ�ţ���һ�¿�ʼ����flash
	data_len=25;
	if (len < data_len)
    {
        return 0;
    }

	buf[i++] = COMM_PROTOCOL_HEAD;
	buf[i++] = (data_len)%256;
	buf[i++] = (data_len)/256;
	buf[i++] = fuc_param;//������	
	buf[i++] = iap_param.ID%256;//����lora��ַ
	buf[i++] = iap_param.ID/256;
	buf[i++] = (iap_param.Channel-410);
	
	buf[i++] = system_param.adc_data_l;//ADC��λ
	buf[i++] = system_param.adc_data_h;//ADC��λ
    buf[i++] = hardwave_version_marker;//Ӳ���汾
	buf[i++] = softwave_version_marker;//����汾
	buf[i++] = decice_id;//�豸����
    buf[i++] = system_param.start_time[0];//��ʼʱ��   ʱ
	buf[i++] = system_param.start_time[1];//��ʼʱ��   ��
    buf[i++] = system_param.start_time[2];//��ʼʱ��   ��
	buf[i++] = system_param.led_period_interval[2];//�����˸   ��λ
    buf[i++] = system_param.led_period_interval[1];//�����˸   ��λ
    buf[i++] = system_param.led_period_interval[0];//�����˸   ��λ
	buf[i++] = system_param.RTC_WakeUp_Time;//RTC����ʱ��
	
	buf[i++] = run_time&0x000000ff;//����ʱ��  ��λ
	buf[i++] = (run_time&0x0000ff00) >> 8;
	buf[i++] = (run_time&0x00ff0000) >> 16;
	buf[i++] = (run_time&0xff000000) >> 24;//����ʱ��  ��λ
		
	buf[i++] = Get_Sum8(buf + 1, data_len - 3);
	buf[i++] = COMM_PROTOCOL_TAIL;
	return i;
}
uint32_t gps_param_read(uint8_t* buf,uint8_t fuc_param, uint32_t len)
{
 u32 data_len;
	int i=0;
	//��ѯ��ǰID �ŵ��Ƿ��flash�浵��һ�£�һ����ʼͨ�ţ���һ�¿�ʼ����flash
	data_len=25;
	if (len < data_len)
    {
        return 0;
    }

	buf[i++] = COMM_PROTOCOL_HEAD;
	buf[i++] = (data_len)%256;
	buf[i++] = (data_len)/256;
	buf[i++] = fuc_param;//������	
	buf[i++] = iap_param.ID%256;//����lora��ַ
	buf[i++] = iap_param.ID/256;
	buf[i++] = (iap_param.Channel-410);
	
		
	for(int j=7;j >= 0; j--)
	{
    buf[i++] = system_param.latitude[j];//γ�ȵ�λ��ǰ
	}		
	for(int j=7;j >= 0; j--)
	{
    buf[i++] = system_param.longitude[j];//���ȵ�λ��ǰ
	}		
	
	buf[i++] = Get_Sum8(buf + 1, data_len - 3);
	buf[i++] = COMM_PROTOCOL_TAIL;
	return i;
}
uint32_t app_param_read(uint8_t* buf,uint8_t fuc_param, uint32_t len)
{
	u32 data_len;
	int i=0;
	//��ѯ��ǰID �ŵ��Ƿ��flash�浵��һ�£�һ����ʼͨ�ţ���һ�¿�ʼ����flash
	data_len=29;
	if (len < data_len)
    {
        return 0;
    }

	buf[i++] = COMM_PROTOCOL_HEAD;
	buf[i++] = (data_len)%256;
	buf[i++] = (data_len)/256;
	buf[i++] = fuc_param;//������	
	buf[i++] = iap_param.ID%256;
	buf[i++] = iap_param.ID/256;
	buf[i++] = (iap_param.Channel-410);
	buf[i++] = iap_param.GatewayID%256;
	buf[i++] = iap_param.GatewayID/256;	
	

		
	//buf[i++] = system_param.change_RTC_WakeUp_period;//������˸ʱ��ɹ���־
	buf[i++] = system_param.adc_data_l;//ADC��λ
	buf[i++] = system_param.adc_data_h;//ADC��λ
		
//	buf[i++] = system_param.change_RTC_SetAlarm_ok;//(iap_param.ChipUniqueID[1]>>8)&0xFF;	
	buf[i++] = 0x00;//iap_param.ChipUniqueID[2]&0xFF;
	buf[i++] = 0x00;//(iap_param.ChipUniqueID[2]>>8)&0xFF;	
	buf[i++] = 0x00;//iap_param.ChipUniqueID[3]&0xFF;
	buf[i++] = 0x00;//(iap_param.ChipUniqueID[3]>>8)&0xFF;	
	buf[i++] = hardwave_version_marker;//iap_param.ChipUniqueID[4]&0xFF;
	buf[i++] = softwave_version_marker;//(iap_param.ChipUniqueID[4]>>8)&0xFF;
	buf[i++] = system_param.latitude[0];//γ�ȶ�iap_param.ChipUniqueID[5]&0xFF;
	buf[i++] = system_param.latitude[1];//γ�ȷ�(iap_param.ChipUniqueID[5]>>8)&0xFF;
	buf[i++] = system_param.latitude[2];//γ����(u8)iap_param.lora_rssi;//iap_param.Lora_fourth;
	buf[i++] = system_param.longitude[0];//���ȶ�((u8)(iap_param.lora_rssi>>8));//iap_param.Lora_six;
	buf[i++] = system_param.longitude[1];//���ȷ�iap_param.IAP_flag;
	buf[i++] = system_param.longitude[2];//������(u8)iap_param.lora_snr;
	buf[i++] = system_param.maker_run_state;//�������״̬���óɹ���־
	buf[i++] = decice_id;//iap_param.version;
	buf[i++] = Get_Sum8(buf + 1, data_len - 3);
	buf[i++] = COMM_PROTOCOL_TAIL;
	return i;
}
void ADC_ChnCfg(uint32_t chn)
{
	static uint8_t lastchn=255;
	ADC_ChannelConfTypeDef sConfig;
	HAL_ADC_Stop(&hadc1);
	if(lastchn!=chn)
	{
		/*-2- Configure ADC regular channel */
		sConfig.Channel = (uint32_t)chn;//ADC1_CHANNEL6;//ADC1_CHANNEL0;
		sConfig.Rank = ADC_REGULAR_RANK_1;
		sConfig.SamplingTime = ADC_SAMPLETIME_247CYCLES_5;//ADC_SAMPLETIME_12CYCLES_5;
		sConfig.SingleDiff = ADC_SINGLE_ENDED;
		sConfig.OffsetNumber = ADC_OFFSET_NONE;
		sConfig.Offset = 0;
		HAL_ADC_ConfigChannel(&hadc1, &sConfig);
	}
	lastchn = chn;
}
void ADC_GetVol(uint32_t chn,uint16_t* rp,uint8_t num)
{
		uint8_t temp;
	  uint16_t adc_sum;
		ADC_ChnCfg(chn);
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 10);
		if((HAL_ADC_GetState(&hadc1) & HAL_ADC_STATE_REG_EOC) == HAL_ADC_STATE_REG_EOC)//HAL_ADC_STATE_EOC_REG
		{
			for(temp=0;temp < num;temp++)
			{
			  adc_sum += HAL_ADC_GetValue(&hadc1);
				HAL_Delay(1);
			}
			*rp=adc_sum/num;//HAL_ADC_GetValue(&hadc1);
			adc_sum = 0;
		}
}
/*
*\Function      RTC����Ƶ��У׼���򣬹̶����У׼
*\Description   
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.07.18    Ver 1.0                  
				����������
*/
HAL_StatusTypeDef HAL_RTC_SetSmoothCalib(RTC_HandleTypeDef* hrtc, uint32_t SmoothCalibValue)
{
  uint32_t tickstart = 0;
  /* Process Locked */ 
  __HAL_LOCK(hrtc);

  hrtc->State = HAL_RTC_STATE_BUSY;

  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);

  /* check if a calibration is pending*/
  if((hrtc->Instance->ISR & RTC_ISR_RECALPF) != RESET)
  {
    tickstart = HAL_GetTick();

    /* check if a calibration is pending*/
    while((hrtc->Instance->ISR & RTC_ISR_RECALPF) != RESET)
    {
      if((HAL_GetTick() - tickstart ) > RTC_TIMEOUT_VALUE)
      {
        /* Enable the write protection for RTC registers */
        __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

        /* Change RTC state */
        hrtc->State = HAL_RTC_STATE_TIMEOUT; 
        
        /* Process Unlocked */
        __HAL_UNLOCK(hrtc);

        return HAL_TIMEOUT;
      }
    }
  }
	/* Configure the Smooth calibration settings */
  hrtc->Instance->CALR = (uint32_t)SmoothCalibValue;

  /* Enable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

  /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY; 

  /* Process Unlocked */
  __HAL_UNLOCK(hrtc);

  return HAL_OK;
}
/*
*\Function      calibration RTC
*\Description   
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.07.18    Ver 1.0                  
				����������
*/
void HAL_RTC_Calibration(void)
{
	uint16_t currentTemperature = 0;
	static uint16_t lastTemperature = 25;
	uint16_t adc_tempsensor=0;
	uint32_t calrRegisterValue = 0;
	float deltaPPM = 0;
	double ppmValue = 0;
	double calValue = 0;
	
	cal_num++;
	if(cal_num >= 0x90) 
    {
	 cal_num = 0;
	}
    LoRa_Stop(&LoRa_PP);//lora�ڴ�״̬��Ӱ��ADC�ɼ�����
	u16VrefintCal =  (*(__IO uint16_t*) 0x1FFF75AA);
	ADC_GetVol(ADC_CHANNEL_VREFINT,&u16VrefintVal,2);
	VddVal = (3.0 * u16VrefintCal / u16VrefintVal) - vddval_offset;
	ADC_GetVol(ADC_CHANNEL_TEMPSENSOR,&adc_tempsensor,2);
	
	currentTemperature = __LL_ADC_CALC_TEMPERATURE(VddVal*1000, adc_tempsensor, LL_ADC_RESOLUTION_12B);
	//if(currentTemperature != lastTemperature)
	{
		#ifdef MC306_G_06Q_32768
		deltaPPM = -0.04 * (currentTemperature - ROOM_TEMPERATURE) * (currentTemperature - ROOM_TEMPERATURE);
		#endif
		ppmValue = deltaPPM + ROOM_TEMPERATURE_PPM;
		calValue = ppmValue / 0.954;
		if(calValue >= 0)
		{
			if( (calValue - (uint32_t)(ppmValue / 0.954)) >= 0.5)
		    {
			 calValue = calValue + 1;
		    }
			calrRegisterValue = (uint32_t)(calValue);
			if(cal_num%2 == 0)
			{
			  calrRegisterValue = calrRegisterValue + 1;
			}
			else if(cal_num%2 == 1)
			{
			  calrRegisterValue = calrRegisterValue - 1;
			}
			HAL_RTC_SetSmoothCalib(&RtcHandle,calrRegisterValue);
		}
		else
		{
			if( ((int)(ppmValue / 0.954)) - calValue >= 0.5)
			{
				calValue = (int)calValue - 1;
			}
			calrRegisterValue = (uint32_t)(0x8000 + 512 + calValue);
			if(cal_num%2 == 0)
			{
			  calrRegisterValue = calrRegisterValue + 1;
			}
			else if(cal_num%2 == 1)
			{
			  calrRegisterValue = calrRegisterValue - 1;
			}
			HAL_RTC_SetSmoothCalib(&RtcHandle,calrRegisterValue);
		}
	  lastTemperature = currentTemperature;
	}
	LoRa_Start(&LoRa_PP);
}
/* gps uart USER CODE BEGIN 1 */
uint8_t UART1_RxBuf_GPS[UART1_RxBuf_SIZE];
uint8_t UART1_RxFlag_GPS = 0;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	uint8_t temp;
	temp= huart->Instance->RDR;
	static uint8_t GPS_State=0;
	static uint32_t rx_write;   //д��ָ��
	static uint8_t cs;
	GPS_INFO GPS_Data;
	switch(GPS_State)
	{
		case Start:
		{
			if(temp == '$') 
			{
				GPS_State = G;
				rx_write = 0;
				cs = 0;
				UART1_RxBuf_GPS[rx_write++] = temp;
			}
			else GPS_State = Start; 
		}
		break;
		case G:
		{
			if(temp == 'G')
			{
				GPS_State = P;
				UART1_RxBuf_GPS[rx_write++] = temp;
				cs ^= temp;
			}
			else GPS_State = Start; 
		}
		break;
		case P:
		{
			if(temp == 'N')
			{
				GPS_State = R;
				UART1_RxBuf_GPS[rx_write++] = temp;
				cs ^= temp;
			}
			else GPS_State = Start;
		}
		break;
		case R:
		{
			if(temp == 'R')
			{
				GPS_State = M;
				UART1_RxBuf_GPS[rx_write++] = temp;
				cs ^= temp;
			}
			else GPS_State = Start; 
		}
		break;
		case M:
		{
			if(temp == 'M')
			{
				GPS_State = C;
				UART1_RxBuf_GPS[rx_write++] = temp;
				cs ^= temp;
			}
			else GPS_State = Start; 		
		}
		break;
		case C:
		{
			if(temp == 'C')
			{
				GPS_State = Data;
				UART1_RxBuf_GPS[rx_write++] = temp;
				cs ^= temp;
			}
			else GPS_State = Start; 
		}
		break;
		case Data:
		{
			if(temp == '*')
			{ 
				GPS_State = Check0;
				UART1_RxBuf_GPS[rx_write++] = temp;
			}
			else
			{
				UART1_RxBuf_GPS[rx_write++] = temp;
				cs ^= temp;
				if(rx_write >= UART1_RxBuf_SIZE - 2) 
				{
						GPS_State = Start;  
				}
			}
		}
		break;
		case Check0:
		{
			GPS_State = Check1;
			UART1_RxBuf_GPS[rx_write++] = temp;
		}
		break;
		case Check1:   //*hh
		{
			GPS_State = Start;
			UART1_RxBuf_GPS[rx_write++] = temp;
			UART1_RxBuf_GPS[rx_write] = 0;//���һ���ֽ���Ϊ0,��Ϊ�ַ����ô���
			//У�鴦��
			if(cs == ((UART1_RxBuf_GPS[rx_write - 2] -'0') * 16 + (UART1_RxBuf_GPS[rx_write - 1] - '0')))
			{
				//����һ֡GPRMC���ݱ���������$GPRMC,....*hh ����Ļس����к���,����Ϊ��$��ʼ��*hh���ܳ�
				if (GPS_RMC_Parse(&GPS_Data) != TRUE)
				{
					//_Error_Handler(__FILE__, __LINE__);
				}

			}
			
		}
		break;		
		default:break;
	
	}
	
}
/**
$GPRMC,092927.000,A,2235.9058,N,11400.0518,E,0.000,74.11,151216,,D*49
  * @note   This function handles prase gprmc                
  * @param  buffer: usart receive data 
  * @param  GPS: gps handles
  */
bool GPS_RMC_Parse(GPS_INFO *GPS)
{
	  uint32_t count = 0;
    DataNode rmc_info[30];

    count = StrSplit(UART1_RxBuf_GPS, ',', 0x00, rmc_info, countof(rmc_info));
    if (count == 0 || count != 13)
    {
        return FALSE;
    }
	    //hhmmss.sss
    if (rmc_info[1].Data && rmc_info[9].Data && rmc_info[3].Data && rmc_info[5].Data)
    {
//        GPS->InRTC.Hour   = 10 * (rmc_info[1].Data[0] - '0') + (rmc_info[1].Data[1] - '0');
//        GPS->InRTC.Minute = 10 * (rmc_info[1].Data[2] - '0') + (rmc_info[1].Data[3] - '0');
//        GPS->InRTC.Second = 10 * (rmc_info[1].Data[4] - '0') + (rmc_info[1].Data[5] - '0');

//        GPS->InRTC.Day   = 10 * (rmc_info[9].Data[0] - '0') + (rmc_info[9].Data[1] - '0');
//        GPS->InRTC.Month = 10 * (rmc_info[9].Data[2] - '0') + (rmc_info[9].Data[3] - '0');
//        GPS->InRTC.Year  = 10 * (rmc_info[9].Data[4] - '0') + (rmc_info[9].Data[5] - '0');
//        
//        //UTC + 8Сʱ ����ʱ��
//        TimeDECAddHour(&GPS->InRTC, 8);

//        TimeDECAddSecond(&GPS->InRTC, 1);//�˴���1s�����������ж���������Ҫ��1s��
			
				 //���뾭γ��   3642.4240,N,11910.8205,E,
					//γ��
//			  GPS->latitude = (double)( 1000 * (rmc_info[3].Data[0] - '0') + 100*(rmc_info[3].Data[1] - '0') + 10*(rmc_info[3].Data[2] - '0') + \
//				                (rmc_info[3].Data[3] - '0') + 0.1 * (rmc_info[3].Data[5] - '0') + 0.01*(rmc_info[3].Data[6] - '0') + \
//				                0.001*(rmc_info[3].Data[7] - '0')+ 0.0001*(rmc_info[3].Data[8] - '0') );
//				
//					//����			
//        GPS->longitude = (double)( 10000 * (rmc_info[5].Data[0] - '0') + 1000*(rmc_info[5].Data[1] - '0') + 100*(rmc_info[5].Data[2] - '0') + \
//				                 10 * (rmc_info[5].Data[3] - '0') + (rmc_info[5].Data[4] - '0') + 0.1 * (rmc_info[5].Data[6] - '0') + \
//				                 0.01*(rmc_info[5].Data[7] - '0')+ 0.001*(rmc_info[5].Data[8] - '0')+ 0.0001*(rmc_info[5].Data[9] - '0') );
				GPS->latitude = (double)( 10 * (rmc_info[3].Data[0] - '0') + 1*(rmc_info[3].Data[1] - '0') + 0.1*(rmc_info[3].Data[2] - '0') + \
				                (rmc_info[3].Data[3] - '0') + 0.001 * (rmc_info[3].Data[5] - '0') + 0.0001*(rmc_info[3].Data[6] - '0') + \
				                0.00001*(rmc_info[3].Data[7] - '0')+ 0.000001*(rmc_info[3].Data[8] - '0') );
				
					//����			
        GPS->longitude = (double)( 100 * (rmc_info[5].Data[0] - '0') + 10*(rmc_info[5].Data[1] - '0') + 1*(rmc_info[5].Data[2] - '0') + \
				                 0.1 * (rmc_info[5].Data[3] - '0') + 0.01*(rmc_info[5].Data[4] - '0') + 0.001 * (rmc_info[5].Data[6] - '0') + \
				                 0.0001*(rmc_info[5].Data[7] - '0')+ 0.00001*(rmc_info[5].Data[8] - '0')+ 0.000001*(rmc_info[5].Data[9] - '0') );
				//����ת��
        memcpy( system_param.latitude,&GPS->latitude,sizeof(double) );  //γ��
				memcpy( system_param.longitude,&GPS->longitude,sizeof(double) );//����
				
				app_param_save();//save to flash
				/* �ɹ��������ݺ�ص���ʱ�жϲ���ֹʹ��USART1  ,�ص�GPS  */
				GPS_Timeout.Enable=0;
				GPS_Timeout.Counter=0;
				Read_GPS_Flag = 0;
				if(Lora_Recive_Flag == 0 && ShouShi.state == 0)
				{
				 Lora_tens_flag = 0;
				}
				HAL_NVIC_DisableIRQ(USART1_IRQn);
				GPS_PowerOFF;
				stm32_uart_disinit(&UartTTL); //disable usart1   �������󹦺�
				Marker_State = State_Lp_Run;

        return TRUE;
    }

		else
		{
			return FALSE;
		}
		
}
void Lora_SendGPS_lastData(void)
{
		u32 send_len=0;
		send_len = gps_param_read(OTASendBuf,0x14, sizeof(OTASendBuf));		
		if (send_len)
		{
			LoRa_Send(&LoRa_PP, iap_param.src_addr, OTASendBuf, send_len);  //system_param.src_addr Ŀ��lora��ַ
		}
}
void HAL_SYSTICK_Callback(void)
{
	if(LED_Timeout.Enable == 1)
	{
	  LED_Timeout.Counter ++;
	  if( LED_Timeout.Counter > LED_Timeout.Threshold - 1)//��ʱ��ص�LED
		{
			LED_Timeout.Enable=0;
			LED_Timeout.Counter=0;
			LED1_OFF;
			LED2_OFF;
			led_flag = 0;
			Marker_State = State_Lp_Run;
		}
	}

}
/*
*\Function      flash_save
*\Description   ��flash�б�������
*\Parameter     
*\Parameter     
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.07.19
*               ����������  
*/
void app_param_save(void)
{
	static FLASH_EraseInitTypeDef EraseInitStruct_app;
	uint32_t addr = FLASH_APP_PARAM_ADDR;
	uint32_t FirstPage = 0;
	uint32_t NbOfPages = 0;
	uint32_t PAGEError = 0;
	uint32_t BankNumber = 0;

	HAL_FLASH_Unlock();
		/* ����������� */
	/* Clear OPTVERR bit set on virgin samples */
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR); 

			/* Get the bank */
	BankNumber = GetBank(addr);
	FirstPage =GetPage(addr);
	NbOfPages = GetPage(addr) - FirstPage + 1;
	/* Fill EraseInit structure*/
	EraseInitStruct_app.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct_app.Banks     = BankNumber;
	EraseInitStruct_app.Page      = FirstPage;
	EraseInitStruct_app.NbPages	  = NbOfPages;
	FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);
	if (HAL_FLASHEx_Erase(&EraseInitStruct_app, &PAGEError) != HAL_OK)
	{
//		printf("erase error\n");
	}	
	else
	{
//		printf("erase ok\n");
	}
	
    Flash_Write(addr, (uint8_t*)&system_param,sizeof(system_param));
    addr += sizeof(system_param);

    /* FLASH ���� */
    HAL_FLASH_Lock();

}
/*
*\Function      flash_reae
*\Description   ��flash�ж�ȡ����
*\Parameter     
*\Parameter     
*\Parameter     
*\Return        void
*\Note          
*\Log           2018.07.19
*               ����������  
*/
void app_param_load(void)
{
	uint32_t addr = FLASH_APP_PARAM_ADDR;

	Flash_Read(addr, (uint8_t*)&system_param, sizeof(system_param));
	addr += sizeof(system_param);
//�������ݺ󣬼�����˸����ʱ��s
	LED_Period = (system_param.led_period_interval[0]<<16) + (system_param.led_period_interval[1]<<8) + system_param.led_period_interval[2];
	start_time[0] = system_param.start_time[0];//��ȡflash�е���ʼʱ������˸���ʱ��
	start_time[1] = system_param.start_time[1];
	start_time[2] = system_param.start_time[2];
	ROOM_TEMPERATURE_PPM = system_param.ROOM_TEMPERATURE_PPM;//RTCУ׼ֵ��FLASH�ж�ȡ����
}
void app_param_default(void)
{
  memset(&system_param, 0, sizeof(system_param));
	//��һ���ϵ�����Ӳ���汾�ż��豸��д��flash����
	system_param.Power_First_Flag = DEV_FIRST_POWER_FLAG;
	system_param.RTC_WakeUp_Time=0x0C;//Ĭ��RTC_WAKEUP����ʱ��12S     0x1e;//Ĭ��RTC_WAKEUP����ʱ��30S
	system_param.start_time[0]= 0;//Ĭ����ʼʱ��00:00:00
	system_param.start_time[1]= 0;
	system_param.start_time[2]= 0;
	system_param.led_period_interval[0]= 0x00;//Ĭ����˸���1min��˸����ʼʱ��00:00:00,��700ms
	system_param.led_period_interval[1]= 0x02;
	system_param.led_period_interval[2]= 0x58;
	system_param.ROOM_TEMPERATURE_PPM = 82;//RTC  PPMֵ
	app_param_save();
}
void It_Judge(void)
{
	if(AlarmA_Flag == 1)
	{
		/* Get the RTC current Time */
		HAL_RTC_GetTime(&RtcHandle, &RTC_TimeStructe, RTC_FORMAT_BIN);
		/* Get the RTC current Date */
		HAL_RTC_GetDate(&RtcHandle, &RTC_DateStructe, RTC_FORMAT_BIN);
		
		Now_Time_Start=(RTC_TimeStructe.Hours*3600)+(RTC_TimeStructe.Minutes*60)+RTC_TimeStructe.Seconds;// ����ʱ��ת��Ϊs

		Watchdog_Refresh();		
		AlarmA_Flag=0;
	//	LED ��˸�жϿ�ʼ
		if(led_flag == 0)
		{
			LED_Run_SingleON;
			HAL_Delay(839);
			LoRa_Set();//LoRa_Start(&LoRa_PP);//
			HAL_RTC_Calibration();
			LED_Run_OFF;
		}
		if(led_flag == 1)
		{
			led_flag = 0;
		}
		AlarmA_Interval_Set();
	    //	LED ��˸�жϽ���
		//  �̶�ʱ����޸�lora�������,һ����ʱʱ�䣺 00:00/03:00/06:00/09:00/12:00/15:00/18:00/21:00
		//if(Now_Time_Start == 0 || Now_Time_Start == 21600 || Now_Time_Start == 43200 ||Now_Time_Start == 64800 )
		if(Now_Time_Start == 0 || Now_Time_Start == 10800 || Now_Time_Start == 21600 || Now_Time_Start == 32400 || Now_Time_Start == 43200 || Now_Time_Start == 54000 || Now_Time_Start == 64800 || Now_Time_Start == 75600)
		{  
			LoRa_Set();//LoRa_Start(&LoRa_PP);//
			RTC_SyncTime_Last=LoRa_PP.RTCSync_TIME.T.Year+LoRa_PP.RTCSync_TIME.T.Month+LoRa_PP.RTCSync_TIME.T.Day+LoRa_PP.RTCSync_TIME.T.Hour+LoRa_PP.RTCSync_TIME.T.Minute+LoRa_PP.RTCSync_TIME.T.Second+LoRa_PP.RTCSync_TIME.SubSecs;
            RTC_SyncTime_Current = RTC_SyncTime_Last;
			ShouShi.state=1;
			ShouShi.count=0;
			Lora_tens_flag=1;
			AlarmB_Flag=0;
		}
	}
	if(RTC_WakeUp_It == 1)//12S INTO
	{
		Watchdog_Refresh(); //ÿ12sι��
		RTC_WakeUp_It=0;
		run_time += system_param.RTC_WakeUp_Time;//����ʱ��ͳ��
		if(run_time >= 0xffffff00) 
		{
		  run_time = 0;//��ʱ������
		}
		get_Set_Alarm_Time_num++;
		if(get_Set_Alarm_Time_num == 25)//5min get time
		{
			get_Set_Alarm_Time();
			get_Set_Alarm_Time_num=0;
		}
		if(Lora_Recive_Flag == 1)//lora���յ�����ָ���ʼ��ʱ10Min
		{
		  Lora_Recive_Count++;
			Lora_tens_flag = 1;
			if(Lora_Recive_Count >= 50)
			{
			  Lora_Recive_Flag = 0;
				Lora_Recive_Count = 0;
				if(ShouShi.state == 0)
				{
				 LoRa_Set();//LoRa_Start(&LoRa_PP);//
				 LoRa_Stop(&LoRa_PP);//10min��Lora���¹ص�
				 Lora_tens_flag = 0;
				}
			}
		}
		if(GPS_Timeout.Enable == 1)//GPS 5min��ʱ
  	    {
	  	    GPS_Timeout.Counter++;
			if( GPS_Timeout.Counter >= GPS_Timeout.Threshold - 1)//��ʱ��ص�GPS��USART1
		    {
				GPS_Timeout.Enable=0;
				GPS_Timeout.Counter=0;
				Read_GPS_Flag = 0;
				if(Lora_Recive_Flag == 0 && ShouShi.state == 0)
				{
				 Lora_tens_flag = 0;
				}
				HAL_NVIC_DisableIRQ(USART1_IRQn);
				GPS_PowerOFF;
				stm32_uart_disinit(&UartTTL); //disable usart1   �������󹦺�
				Marker_State = State_Lp_Run;
		     }
	    }
		if(lora_time_start == 1)//ÿ�ο���30min��ʱ
		{
			ShouShi.state=1;
			Lora_tens_flag=1;
			AlarmB_Flag=0;
		}
		if(ShouShi.state == 1)//30min��ʱ��ʱ
		{
		  ShouShi.count++;
		  LoRa_Get_RTCSyncLast(&LoRa_PP);
		  RTC_SyncTime_Current=LoRa_PP.RTCSync_TIME.T.Year+LoRa_PP.RTCSync_TIME.T.Month+LoRa_PP.RTCSync_TIME.T.Day+LoRa_PP.RTCSync_TIME.T.Hour+LoRa_PP.RTCSync_TIME.T.Minute+LoRa_PP.RTCSync_TIME.T.Second+LoRa_PP.RTCSync_TIME.SubSecs;
			if(RTC_SyncTime_Last != RTC_SyncTime_Current)
			{
			  RTC_SyncTime_Last=LoRa_PP.RTCSync_TIME.T.Year+LoRa_PP.RTCSync_TIME.T.Month+LoRa_PP.RTCSync_TIME.T.Day+LoRa_PP.RTCSync_TIME.T.Hour+LoRa_PP.RTCSync_TIME.T.Minute+LoRa_PP.RTCSync_TIME.T.Second+LoRa_PP.RTCSync_TIME.SubSecs;

				if(GPS_Timeout.Enable == 0 && Lora_Recive_Flag == 0)
				{
					LoRa_Set();//LoRa_Start(&LoRa_PP);//��ʱ�ɹ�������ʱʱ���
					LoRa_Stop(&LoRa_PP);
			    Lora_tens_flag=0;
				}
			  ShouShi.state=0;
				ShouShi.count=0;
				lora_time_start=0;
			}
			if(ShouShi.count >= 150)
			{
				LoRa_Set();//LoRa_Start(&LoRa_PP);//��ʱ��ʱ������ʱ���
			  LoRa_Stop(&LoRa_PP);
				Lora_tens_flag=0;
		  	Lora_Recive_Flag = 0;
			  Lora_Recive_Count = 0;
			  ShouShi.state=0;
				ShouShi.count=0;
				lora_time_start=0;
			}
		}
		if(Lora_tens_flag == 0)//���10S��lora���ֽ���ͨѶ����
		{
		  LoRa_Set();//LoRa_Start(&LoRa_PP);//
		  get_Set_Alarm_Time_AlarmB();
		#ifdef Target_APP
		  Lp_Run();//����͹���1.5s
		#endif
		}
	}
	if(AlarmB_Flag == 1)
	{
		Watchdog_Refresh(); //ÿ12sι��
		AlarmB_Flag=0;
		if(Lora_Recive_Flag == 0)
		{
		 LoRa_Set();//LoRa_Start(&LoRa_PP);//
         LoRa_Stop(&LoRa_PP);
		}
		if(Lora_Recive_Flag == 1)
		{
		 Lora_tens_flag = 1;
		}
	}
}
/* USER CODE END 1 */