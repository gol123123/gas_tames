#include "stm32f10x.h"

#define BLUETOOTH	1

#ifdef BLUETOOTH
#include <stdio.h>
#include <CRC16.c>
#include <retarget.c>
#endif

void InitAll (void);
void delay_ms (uint16_t delay);
void delay_us (uint16_t delay);

void TIM2_IRQHandler (void);
void TIM3_IRQHandler (void);
uint8_t  RtcInit  (void);
void setup_time (void);
void print (uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4);
#ifdef BLUETOOTH
void USART1_IRQHandler (void);
uint8_t getchar2 (void);
int sendchar (int data);
void setup_bt (void);
#endif

#define buzz_on()   	GPIO_SetBits(GPIOB, GPIO_Pin_8)	//пищало
#define buzz_off()   	GPIO_ResetBits(GPIOB, GPIO_Pin_8)
#define buzz_inv()   	{if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8)) GPIO_ResetBits(GPIOB, GPIO_Pin_8); else GPIO_SetBits(GPIOB, GPIO_Pin_8);}

#define but1   			!(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7))
#define but2   			!(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6))
#define but3   			!(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5))
#define but4   			!(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4))
#define but5   			!(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3))

#ifdef BLUETOOTH

#define led_on()   		GPIO_SetBits(GPIOB, GPIO_Pin_11)	// светик
#define led_off()   	GPIO_ResetBits(GPIOB, GPIO_Pin_11)
#define led_inv()   	{if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11)) GPIO_ResetBits(GPIOB, GPIO_Pin_11); else GPIO_SetBits(GPIOB, GPIO_Pin_11);}

uint8_t bt_mode = 0;
#define bt_reset() 		{GPIO_ResetBits(GPIOA, GPIO_Pin_6); delay_ms(20); GPIO_SetBits(GPIOA, GPIO_Pin_6);} 
#define bt_mode_cmd() 	{bt_mode = 0; GPIO_SetBits(GPIOB, GPIO_Pin_2); delay_ms(5); bt_reset();}
#define bt_mode_data() 	{bt_mode = 1; GPIO_ResetBits(GPIOB, GPIO_Pin_2); delay_ms(5); bt_reset();}

#define RX_BUFFER_SIZE2 32
uint8_t rx_buffer2[RX_BUFFER_SIZE2];
uint8_t rx_wr_index2,rx_rd_index2,rx_counter2;
uint8_t data[12];
char crc_buf[4];
uint32_t crc;
uint8_t i, j;

#endif

uint16_t time_delay_ms = 0;
uint16_t but3_cnt, led_cnt;
uint8_t long_press = 0;	          
uint8_t mode = 0;
uint8_t sound = 0;
uint8_t bright = 255;

uint8_t symbol[12] =
	{
	//fhabgcde
	0xB7,   //0 
	0x14,   //1 
	0x3B,   //2 
	0x3E,   //3 
	0x9C,   //4 
	0xAE,   //5 
	0xAF,   //6 
	0x34,   //7 
	0xBF,   //8 
	0xBE, 	//9
	0x08,	//-
	0x00 
	}; 
uint8_t display_bytes[4];

typedef struct
	{
	uint8_t hour;
	uint8_t minutes;
	uint8_t seconds;
	} clock_type;

uint32_t clock_coder(clock_type * clock)
	{
	uint32_t tmp;

	tmp = clock->hour * 3600;
	tmp += clock->minutes * 60;
	return tmp + clock->seconds;
	}

void clock_decoder(uint32_t value, clock_type * clock)
	{
	clock->hour = (value / 3600) % 24;
	clock->minutes = (value % 3600) / 60;
	clock->seconds = (value % 3600) % 60;
	}

int main(void) 
	{
	clock_type clock;

	InitAll();

	sound = 50;

	#ifdef BLUETOOTH
	bt_mode_data();
	#endif

	while (1)
        {
		if (but3 && mode == 0)
			{
			while (but3 && long_press == 0) {};
			if (but3 && long_press == 1) {sound = 50;}
			while (but3 && long_press == 1) {};
			if (but3 && long_press == 2) {sound = 50; delay_ms(200); sound = 50;}
			while (but3 && long_press == 2) {};
			if (but3 && long_press == 3) {sound = 50; delay_ms(200); sound = 50; delay_ms(200); sound = 50;}
			while (but3) {};
			if (long_press == 1) {long_press = 0; setup_time();}
				#ifdef BLUETOOTH
				else if (long_press == 2) {long_press = 0;}
			        	else if (long_press == 3) {long_press = 0; setup_bt();}
				#endif
			                    else {sound = 50; mode = 1; long_press = 0;}
			}
		if (but3 && mode == 1) {sound = 50; mode = 0; while (but3) {};}
		
		if (but1)
			{
			delay_ms(10);
			while (but1)
				{
				if (bright < 255) bright++;
				delay_ms(10);
				}
			}
			
		if (but5)
			{
			delay_ms(10);
			while (but5)
				{
				if (bright > 0) bright--;
				delay_ms(10);
				}
			} 
		
		#ifdef BLUETOOTH
		if (rx_counter2)
			{	
			if (getchar2() == 'T')
            	{
              	delay_ms(150);
				USART_Cmd(USART2, DISABLE);
				for(i=0;i<11;i++) data[i] = 0;	// обнулились  
                i=0;
                while (rx_counter2 && i<=11) {data[i] = getchar2(); i++;};	// загрузили строку в массив
                crc = Crc16 (data, 6);	// посчитали контрольную сумму по первым 6 байтам
                sprintf(crc_buf, "%04X", crc);  // загрузили в буферный массив дл€ удобства сравнени€
				if (data[6] == crc_buf[0] && data[7] == crc_buf[1] && data[8] == crc_buf[2] && data[9] == crc_buf[3])
                	{
                    for (j=0;j<i;j++) data[j] = data[j] - 0x30;	// делаем из ASCII-кодов цифры
                    clock.hour = data[0]*10 + data[1];
                    clock.minutes = data[2]*10 + data[3];
                    clock.seconds = data[4]*10 + data[5];
                    if (clock.hour <= 23 && clock.minutes <= 59 && clock.seconds <= 59)
                    	{
                        RTC_SetCounter(clock_coder(&clock));
						print (10, 10, 10, 10, 0, 0, 0, 0);
						delay_ms(300);
						}
					}
				USART_Cmd(USART2, ENABLE);
				}
			}
		#endif

		clock_decoder(RTC_GetCounter(), &clock);

		if (mode == 0) 
			{
			if (clock.hour >= 10) print (clock.hour/10, clock.hour%10, clock.minutes/10, clock.minutes%10, 0, 1, 0, 0);
				else print (11, clock.hour%10, clock.minutes/10, clock.minutes%10, 0, 1, 0, 0);
			} 
		if (mode == 1) print (11, 11, clock.seconds/10, clock.seconds%10, 0, 1, 0, 0);

		delay_ms(10);
		};
	}

 
//------------------------------------------------------------------------------
 
void InitAll (void) 
	{
  	ErrorStatus HSEStartUpStatus;
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;
	#ifdef BLUETOOTH
	USART_InitTypeDef USART_InitStructure;
	#endif
	
	//GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);
	
	// RCC
	RCC_DeInit();												/* RCC system reset(for debug purpose) */  
	RCC_HSEConfig(RCC_HSE_ON);									/* Enable HSE */  
	HSEStartUpStatus = RCC_WaitForHSEStartUp(); 				/* Wait till HSE is ready */  
	if (HSEStartUpStatus == SUCCESS)   
		{     
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);	/* Enable Prefetch Buffer */        
		FLASH_SetLatency(FLASH_Latency_2);						/* Flash 2 wait state */        
		RCC_HCLKConfig(RCC_SYSCLK_Div1);						/* HCLK = SYSCLK */       
		RCC_PCLK2Config(RCC_HCLK_Div1);       					/* PCLK2 = HCLK */
		RCC_PCLK1Config(RCC_HCLK_Div1);       					/* PCLK1 = HCLK/2 */ 
		RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_6);     	/* PLLCLK = 4MHz * 6 = 24 MHz */   
		RCC_PLLCmd(ENABLE);       								/* Enable PLL */
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);   	/* Wait till PLL is ready */     
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);       		/* Select PLL as system clock source */ 
		while (RCC_GetSYSCLKSource() != 0x08);   				/* Wait till PLL is used as system clock source */
		}

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	// подаем тактирование на систему ремапа и NVIC
	
	// GPIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);
	
	// PORTA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_6;				// Strobe (SPI), BT_Reset
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7;	// SPI
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);

	#ifdef BLUETOOTH
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;				// USART2 Rx
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				// USART2 Tx
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
	#endif

	// PORTB
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);// освобождаем ногу, зан€тую JTAG'ом

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_8|GPIO_Pin_11;				// BT_Mode, Buzzer, LED
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;	// BT_Stat и кнопки
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);

	#ifdef BLUETOOTH
	// USART2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	USART_InitStructure.USART_BaudRate = 38400;
  	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  	USART_InitStructure.USART_StopBits = USART_StopBits_1;
  	USART_InitStructure.USART_Parity = USART_Parity_No ;
  	USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART2, &USART_InitStructure);

	USART_Cmd(USART2, ENABLE);

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
	NVIC_EnableIRQ(USART2_IRQn);
	NVIC_SetPriority(USART2_IRQn, 1);
	#endif

	// “аймеры
	
	// TIM1	- индикаци€ и задержки
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	TIM_PrescalerConfig(TIM1, 12000, TIM_PSCReloadMode_Update);	// предделитель, частота прерываний 2к√ц
	TIM_SetAutoreload(TIM1, 1);									// считать до 1 (1 мс)
	TIM_ARRPreloadConfig(TIM1, ENABLE);							// вкл. автоперезагрузки
	TIM_InternalClockConfig(TIM1);								// внутреннее тактирование
	TIM_CounterModeConfig(TIM1, TIM_CounterMode_Up);			// считать вверх
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);					// вкл. прерывание по переполнению
	TIM_Cmd(TIM1, ENABLE);										// запустили таймер
	
	NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);
	NVIC_SetPriority(TIM1_UP_TIM16_IRQn, 2);	
	
	// TIM3 - выходной Ў»ћ (программный)
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	TIM_PrescalerConfig(TIM3, 400, TIM_PSCReloadMode_Update);	// предделитель, частота прерываний 60к√ц
	TIM_SetAutoreload(TIM3, 1);									// считать до 1 (получаем 30 к√ц)
	TIM_ARRPreloadConfig(TIM3, ENABLE);							// вкл. автоперезагрузки
	TIM_InternalClockConfig(TIM3);								// внутреннее тактирование
	TIM_CounterModeConfig(TIM3, TIM_CounterMode_Up);			// считать вверх
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);					// вкл. прерывание по переполнению
	TIM_Cmd(TIM3, ENABLE);										// запустили таймер

	NVIC_EnableIRQ(TIM3_IRQn);
	NVIC_SetPriority(TIM3_IRQn, 0);								// по Ў»ћу - самое важное прерывание		*/

	// SPI
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx; 
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master; 
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; 
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low; 
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge; 
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; 
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; 
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; 
	//SPI_InitStructure.SPI_CRCPolynomial = 7; 
	SPI_Init(SPI1, &SPI_InitStructure); 
	
	SPI_Cmd(SPI1, ENABLE);

	// IWDG, текущие настройки дают период сброса в 1с
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);	// разрешаем изменение параметров IWDG
	IWDG_SetPrescaler(IWDG_Prescaler_16);			// установили предделитель: вх. частота 40к√ц, предделитель 4,8,16..256
	IWDG_SetReload(0x9C4);							// установили значение счетчика, 0xFFF - максимум
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Disable);	// запретили изменение параметров
	IWDG_Enable();									// включили IWDG 
	
	// RTC
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	
	RtcInit  ();
	}

//*********************************************************************

void print (uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4)
	{
	display_bytes[0] = symbol[d1]; 
	display_bytes[1] = symbol[d2];
	display_bytes[2] = symbol[d3];
	display_bytes[3] = symbol[d4];
	if (p1) display_bytes[0] |= 0x40;			  
	if (p2) display_bytes[1] |= 0x40;
	if (p3) display_bytes[2] |= 0x40; 
	if (p4) display_bytes[3] |= 0x40;
	}

void TIM1_UP_TIM16_IRQHandler (void)
	{
	if (TIM_GetITStatus(TIM1, TIM_SR_UIF) != RESET)
		{
		TIM_ClearITPendingBit(TIM1, TIM_SR_UIF);
		
		IWDG_ReloadCounter();

		if (time_delay_ms) time_delay_ms--;

		if (sound) {sound--; buzz_inv();}
			else buzz_off();

		if (but3)
        	{
            if (++but3_cnt == 1000)
                {
                but3_cnt = 0;
                if (long_press < 3) long_press++;
                } 
            }
            else but3_cnt = 0;
		#ifdef BLUETOOTH
		if (led_cnt) {led_cnt--; led_on();}
			else led_off();
		#endif
		}
	}

void delay_ms(uint16_t delay) 
	{
  	time_delay_ms = delay;
	while (time_delay_ms) __NOP();
    }

uint8_t cnt = 0;
void TIM3_IRQHandler (void)
	{
	if (TIM_GetITStatus(TIM3, TIM_SR_UIF) != RESET)
		{
		TIM_ClearITPendingBit(TIM3, TIM_SR_UIF);
		
		cnt++;
        if (cnt < bright)
			{
			while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
			SPI_I2S_SendData(SPI1, display_bytes[0]);
			while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
	
			while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
			SPI_I2S_SendData(SPI1, display_bytes[1]);
			while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
	
			while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
			SPI_I2S_SendData(SPI1, display_bytes[2]);
			while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
	
			while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
			SPI_I2S_SendData(SPI1, display_bytes[3]);
			while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
	
			GPIO_SetBits( GPIOA, GPIO_Pin_4);
			__NOP(); 
			GPIO_ResetBits( GPIOA, GPIO_Pin_4);
			}
            else
				{
				while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
				SPI_I2S_SendData(SPI1, 0);
				while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
		
				while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
				SPI_I2S_SendData(SPI1, 0);
				while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
		
				while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
				SPI_I2S_SendData(SPI1, 0);
				while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
		
				while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
				SPI_I2S_SendData(SPI1, 0);
				while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
		
				GPIO_SetBits( GPIOA, GPIO_Pin_4);
				__NOP(); 
				GPIO_ResetBits( GPIOA, GPIO_Pin_4);
				}
		}
	}

#ifdef BLUETOOTH
void USART2_IRQHandler (void)
	{
	uint8_t data;
	
	USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	data = USART_ReceiveData(USART2);
	if (bt_mode == 1)
		{
		led_cnt = 50;
		rx_buffer2[rx_wr_index2]=data;
		if (++rx_wr_index2 == RX_BUFFER_SIZE2) rx_wr_index2=0;
		if (++rx_counter2 == RX_BUFFER_SIZE2) rx_counter2=0;
		}
	}

uint8_t getchar2 (void)
	{
	uint8_t data = 0;

	if (rx_counter2)
		{
		data=rx_buffer2[rx_rd_index2];
		if (++rx_rd_index2 == RX_BUFFER_SIZE2) rx_rd_index2=0;
		NVIC_DisableIRQ(USART2_IRQn);
		--rx_counter2;
		NVIC_EnableIRQ(USART2_IRQn);
		return data;
		}
		else return 0xff;
	}

int sendchar (int data)
	{
	uint32_t i = 5000;
	
	USART_SendData(USART2, data);
	while (--i) __NOP();
	return 0;
	}
#endif

//********************************************************************************************
//function инициализаци€ RTC                                                                //
//argument none                                                                             //
//result   1 - инициализаци€ выполнена; 0 - часы уже были инициализированы                  //
//********************************************************************************************
uint8_t  RtcInit  (void)
	{
	//разрешить тактирование модулей управлени€ питанием и управлением резервной областью
	RCC->APB1ENR |= RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN;
	//разрешить доступ к области резервных данных
	PWR->CR |= PWR_CR_DBP;
	//если часы выключены - инициализировать их
	if ((RCC->BDCR & RCC_BDCR_RTCEN) != RCC_BDCR_RTCEN)
  		{
		//выполнить сброс области резервных данных
		RCC->BDCR |=  RCC_BDCR_BDRST;
		RCC->BDCR &= ~RCC_BDCR_BDRST;
		
		//выбрать источником тактовых импульсов внешний кварц 32768 и подать тактирование
		RCC->BDCR |=  RCC_BDCR_RTCEN | RCC_BDCR_RTCSEL_LSE;
		
		RTC->CRL  |=  RTC_CRL_CNF;
		RTC->PRLL  = 0x7FFF;         //регистр делени€ на 32768
		RTC->CRL  &=  ~RTC_CRL_CNF;
		
		//установить бит разрешени€ работы и дождатьс€ установки бита готовности
		RCC->BDCR |= RCC_BDCR_LSEON;
		while ((RCC->BDCR & RCC_BDCR_LSEON) != RCC_BDCR_LSEON){}
		
		RTC->CRL &= (uint16_t)~RTC_CRL_RSF;
		while((RTC->CRL & RTC_CRL_RSF) != RTC_CRL_RSF){}
		
		return 1;
  		}
  	return 0;
	}

void setup_time (void)
	{
	clock_type clock;
	clock_type clock_setup;

	uint16_t i = 0;

	uint8_t setup_cnt = 0;
	uint8_t setup_index = 0;
    uint8_t ch_flag1 = 0;
    uint8_t ch_flag2 = 0;

	clock_decoder(RTC_GetCounter(), &clock_setup);
        
    setup_cnt = 15;
    print (clock_setup.hour/10, clock_setup.hour%10, clock_setup.minutes/10, clock_setup.minutes%10, 0,1,0,0);
        
    while (setup_cnt)
    	{
        setup_cnt--;
		if (setup_index == 0)
            {
            print (clock_setup.hour/10, clock_setup.hour%10, clock_setup.minutes/10, clock_setup.minutes%10, 0,1,0,0); 
			i = 0;
			while (!but1 && !but5 && !but3 && ++i<300) delay_ms(1);
			print (11, 11, clock_setup.minutes/10, clock_setup.minutes%10, 0,1,0,0); 
			i = 0;
			while (!but1 && !but5 && !but3 && ++i<300) delay_ms(1);
			if (but1)
                {
                delay_ms(10);
          		while (but1)
             		{
                    ch_flag1 = 1;
                   	setup_cnt = 15;
                  	if (++clock_setup.hour > 23) clock_setup.hour = 0;        
                   	print (clock_setup.hour/10, clock_setup.hour%10, clock_setup.minutes/10, clock_setup.minutes%10, 0,1,0,0); 
					delay_ms(300);
					}
             	}
                            
            if (but5)
            	{
                delay_ms(10);
                while (but5)
                    {
                    ch_flag1 = 1;
                    setup_cnt = 15;
                    if (--clock_setup.hour > 23) clock_setup.hour = 23;        
                    print (clock_setup.hour/10, clock_setup.hour%10, clock_setup.minutes/10, clock_setup.minutes%10, 0,1,0,0); 
					delay_ms(300);
					}
                }
            }
            
        if (setup_index == 1)
        	{
        	print (clock_setup.hour/10, clock_setup.hour%10, clock_setup.minutes/10, clock_setup.minutes%10, 0,1,0,0); 
			i = 0;
			while (!but1 && !but5 && !but3 && ++i<300) delay_ms(1);
			print (clock_setup.hour/10, clock_setup.hour%10, 11, 11, 0,1,0,0); 
			i = 0;
			while (!but1 && !but5 && !but3 && ++i<300) delay_ms(1);
            if (but1)
            	{
                delay_ms(10);
                while (but1)
                    {
                    ch_flag2 = 1;
                    setup_cnt = 15;
                    if (++clock_setup.minutes > 59) clock_setup.minutes = 0;        
                    print (clock_setup.hour/10, clock_setup.hour%10, clock_setup.minutes/10, clock_setup.minutes%10, 0,1,0,0); 
					delay_ms(300);
					}
                }
                        
            if (but5)
            	{
                delay_ms(10);
                while (but5)
                    {
                    ch_flag2 = 1;
                    setup_cnt = 15;
                    if (--clock_setup.minutes > 59) clock_setup.minutes = 59;        
                    print (clock_setup.hour/10, clock_setup.hour%10, clock_setup.minutes/10, clock_setup.minutes%10, 0,1,0,0); 
					delay_ms(300);
					}
                }
            }
            
        if (but3)
        	{
            delay_ms(10);
            if (but3)
            	{
                if (setup_index == 1)
                	{
                    setup_cnt = 0;
                    if (ch_flag2) 
						{
						clock_decoder(RTC_GetCounter(), &clock);
						clock.minutes = clock_setup.minutes;
						RTC_SetCounter(clock_coder(&clock));
						}
                    sound = 50;
                    }
                if (setup_index == 0)
                	{
                    setup_cnt = 15;
                    setup_index = 1;
                    if (ch_flag1)
						{
						clock_decoder(RTC_GetCounter(), &clock);
						clock.hour = clock_setup.hour;
						RTC_SetCounter(clock_coder(&clock));
						}
                    sound = 50;
                    }
                while (but3);
                }
            }
        }  
	}

#ifdef BLUETOOTH
void setup_bt (void)
	{
	bt_mode_cmd();
	print (10, 10, 10, 10, 0, 0, 0, 0);
	delay_ms(500);
	print (11, 11, 11, 11, 0, 0, 0, 0);
	delay_ms(500);
	print (10, 10, 10, 10, 0, 0, 0, 0);
	delay_ms(500);
	print (11, 11, 11, 11, 0, 0, 0, 0);
	delay_ms(500);
	print (10, 10, 10, 10, 0, 0, 0, 0);
	delay_ms(500);
	print (11, 11, 11, 11, 0, 0, 0, 0);
	delay_ms(500);
	printf ("AT+ORGL\r\n");
	print (10, 10, 10, 10, 0, 0, 0, 0);
	delay_ms(500);
	print (11, 11, 11, 11, 0, 0, 0, 0);
	delay_ms(500);
	printf ("AT+NAME=STM32_CLOCK\r\n");
	print (10, 10, 10, 10, 0, 0, 0, 0);
	delay_ms(500);
	print (11, 11, 11, 11, 0, 0, 0, 0);
	delay_ms(500);
	bt_mode_data();
	sound = 50;
	}
#endif
