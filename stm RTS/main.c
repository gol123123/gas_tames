#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_pwr.h"
#include "stdio.h"
#include "misc.h"

//========================================================================================
unsigned char RTC_Init(void)
{
	// Включить тактирование модулей управления питанием и управлением резервной областью
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	// Разрешить доступ к области резервных данных
	PWR_BackupAccessCmd(ENABLE);
	// Если RTC выключен - инициализировать
	if ((RCC->BDCR & RCC_BDCR_RTCEN) != RCC_BDCR_RTCEN)
	{
		// Сброс данных в резервной области
		RCC_BackupResetCmd(ENABLE);
		RCC_BackupResetCmd(DISABLE);

		// Установить источник тактирования кварц 32768
		RCC_LSEConfig(RCC_LSE_ON);
		while ((RCC->BDCR & RCC_BDCR_LSERDY) != RCC_BDCR_LSERDY) {}
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

		RTC_SetPrescaler(0x7FFF); // Устанавливаем делитель, чтобы часы считали секунды

		// Включаем RTC
		RCC_RTCCLKCmd(ENABLE);

		// Ждем синхронизацию
		RTC_WaitForSynchro();

		return 1;
	}
	return 0;
}

#define JULIAN_DATE_BASE	2440588
typedef struct
{
	uint8_t RTC_Hours;
	uint8_t RTC_Minutes;
	uint8_t RTC_Seconds;
	uint8_t RTC_Date;
	uint8_t RTC_Wday;
	uint8_t RTC_Month;
	uint16_t RTC_Year;
} RTC_DateTimeTypeDef;


// Get current date
void RTC_GetDateTime(uint32_t RTC_Counter, RTC_DateTimeTypeDef* RTC_DateTimeStruct) {
	unsigned long time;
	unsigned long t1, a, b, c, d, e, m;
	int year = 0;
	int mon = 0;
	int wday = 0;
	int mday = 0;
	int hour = 0;
	int min = 0;
	int sec = 0;
	uint64_t jd = 0;;
	uint64_t jdn = 0;

	jd = ((RTC_Counter+43200)/(86400>>1)) + (2440587<<1) + 1;
	jdn = jd>>1;

	time = RTC_Counter;
	t1 = time/60;
	sec = time - t1*60;

	time = t1;
	t1 = time/60;
	min = time - t1*60;

	time = t1;
	t1 = time/24;
	hour = time - t1*24;

	wday = jdn%7;

	a = jdn + 32044;
	b = (4*a+3)/146097;
	c = a - (146097*b)/4;
	d = (4*c+3)/1461;
	e = c - (1461*d)/4;
	m = (5*e+2)/153;
	mday = e - (153*m+2)/5 + 1;
	mon = m + 3 - 12*(m/10);
	year = 100*b + d - 4800 + (m/10);

	RTC_DateTimeStruct->RTC_Year = year;
	RTC_DateTimeStruct->RTC_Month = mon;
	RTC_DateTimeStruct->RTC_Date = mday;
	RTC_DateTimeStruct->RTC_Hours = hour;
	RTC_DateTimeStruct->RTC_Minutes = min;
	RTC_DateTimeStruct->RTC_Seconds = sec;
	RTC_DateTimeStruct->RTC_Wday = wday;
}
/////////////////////////////////////////////////////////////////////////
uint32_t RTC_GetRTC_Counter(RTC_DateTimeTypeDef* RTC_DateTimeStruct) {
	uint8_t a;
	uint16_t y;
	uint8_t m;
	uint32_t JDN;

	a=(14-RTC_DateTimeStruct->RTC_Month)/12;
	y=RTC_DateTimeStruct->RTC_Year+4800-a;
	m=RTC_DateTimeStruct->RTC_Month+(12*a)-3;

	JDN=RTC_DateTimeStruct->RTC_Date;
	JDN+=(153*m+2)/5;
	JDN+=365*y;
	JDN+=y/4;
	JDN+=-y/100;
	JDN+=y/400;
	JDN = JDN -32045;
	JDN = JDN - JULIAN_DATE_BASE;
	JDN*=86400;
	JDN+=(RTC_DateTimeStruct->RTC_Hours*3600);
	JDN+=(RTC_DateTimeStruct->RTC_Minutes*60);
	JDN+=(RTC_DateTimeStruct->RTC_Seconds);

	return JDN;
}

void Delay( void )
{
    for( uint16_t i = 0; i <= 500; i++ )
        for( uint16_t j = 0; j <= 50; j++ );
}

void Delay2( void )
{
    for( uint16_t i = 0; i <= 5000; i++ )
        for( uint16_t j = 0; j <= 500; j++ );
}

GPIO_InitTypeDef GPIO_InitStructure;

void init()		//	инициализация
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_13;	//	использовать выводы PC8 и PC9
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//	на выход
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;			//	использовать вывод PA0
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		//	на вход
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void delay(unsigned long p)		//	задержка
{
	while(p>0){p--;}
}


void Set_Bit_Pin(GPIO_InitTypeDef* PortB, GPIO_InitTypeDef* PortA, RTC_DateTimeTypeDef* RTC_DateTime)
{
  
    int tens_of_second;
    tens_of_second = RTC_DateTime->RTC_Minutes/10;;
    switch(tens_of_second)
    {
         case 1:
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_RESET);
            break;
        case 2:
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_9, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_9, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_RESET);
            break;
        case 3:
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_RESET);
            break;
        case 4:
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_RESET);       
            break;
        case 5:
        GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_RESET);
         Delay();
            break;
        case 6:
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_11, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_11, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_RESET);
            break;
        case 7:
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_RESET);
            break;
        case 8:
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_RESET); 
            break;
        case 9:
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_RESET);
            break;
        case 0:
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_SET);
         GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_RESET);
            break;
    } 

     tens_of_second =   RTC_DateTime->RTC_Minutes%10;
     switch(tens_of_second)
    {
                 case 1:    
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_RESET);
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_RESET);
            break;
        case 2:
          GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_9, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_9, Bit_RESET);
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_RESET);
            break;
        case 3:
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_RESET);
            break;
        case 4:

         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_RESET);
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_RESET);
            break;
        case 5:
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_RESET);
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_RESET);
         Delay();
            break;
        case 6:
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_11, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_11, Bit_RESET);
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_RESET);
            break;
        case 7: 
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET);
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_RESET);
            break;
        case 8:
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_RESET);
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_RESET);
            break;
        case 9:
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_RESET);
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_RESET);
            break;
        case 0:      
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);
         GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_RESET);
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_RESET);
            break;
    } 
    
    int tens_of_Minutes;
    tens_of_Minutes =  RTC_DateTime->RTC_Hours/10;
    switch(tens_of_Minutes)
    {
         case 1:
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_SET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_RESET);
            break;
        case 2:
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_9, Bit_SET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_9, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_RESET);
            break;
        case 3:
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_RESET);
            break;
        case 4:
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_SET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_RESET);
            break;
        case 5:
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_SET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_RESET);
            break;
        case 6:
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_11, Bit_SET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_11, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_RESET);
            break;
        case 7:
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_RESET);
            break;
        case 8:
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_SET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_RESET);
            break;
        case 9:
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_SET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_RESET);
            break;
        case 0:
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_SET);
         GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_SET);
         Delay();
         GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_RESET);
            break;
    } 
    tens_of_Minutes =   RTC_DateTime->RTC_Hours%10;
     switch(tens_of_Minutes)
    {
               case 1:
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET);
            break;
        case 2:
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_9, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_9, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET);
            break;
        case 3:
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET);
            break;
        case 4:
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET);
            break;
        case 5:
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET);
            break;
        case 6:
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_11, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_11, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET); 
            break;
        case 7:
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET);
            break;
        case 8:
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET); 
         Delay();
        case 9:
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET);
            break;
        case 0:
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_SET);
         GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET);
            break;
    }
     
}

void Reset_Bit_PinB(GPIO_InitTypeDef* PortB)
{
    GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_RESET); 
    GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_RESET); 
    GPIO_WriteBit(GPIOB, GPIO_Pin_2, Bit_RESET); 
    GPIO_WriteBit(GPIOB, GPIO_Pin_4, Bit_RESET); 
    GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET); 
    GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_RESET); 
    GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_RESET); 
    GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_RESET); 
    GPIO_WriteBit(GPIOB, GPIO_Pin_9, Bit_RESET); 
    GPIO_WriteBit(GPIOB, GPIO_Pin_10, Bit_RESET); 
    GPIO_WriteBit(GPIOB, GPIO_Pin_11, Bit_RESET);
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);   
    GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_RESET);
    GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET); 
    GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_RESET);     
}
void Reset_Bit_PinA(GPIO_InitTypeDef* PortB)
{
    GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_RESET); 
    GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_RESET);    
}

void copi(RTC_DateTimeTypeDef	RTC_DateTime)
{
    RTC_DateTimeTypeDef RTC_DateTime_copi;
    RTC_DateTime_copi.RTC_Hours = RTC_DateTime.RTC_Hours;
    RTC_DateTime_copi.RTC_Minutes = RTC_DateTime.RTC_Minutes;
    RTC_DateTime_copi.RTC_Seconds = RTC_DateTime.RTC_Seconds;
    RTC_DateTime.RTC_Hours = RTC_DateTime_copi.RTC_Hours;
    RTC_DateTime.RTC_Minutes = RTC_DateTime_copi.RTC_Minutes;
    RTC_DateTime.RTC_Seconds = RTC_DateTime_copi.RTC_Seconds;

}

int main(void)
{
	
	uint32_t RTC_Counter = 0;
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE );  
    RTC_DateTimeTypeDef	RTC_DateTime;
    GPIO_InitTypeDef PortB;                     // Структура с необходимыми полями
    PortB.GPIO_Mode         = GPIO_Mode_Out_PP;    
    PortB.GPIO_Speed        = GPIO_Speed_10MHz;     // По сути это ток который сможет обеспечить вывод
    PortB.GPIO_Pin          = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_12 | GPIO_Pin_11| GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15|
                              GPIO_Pin_10 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_4; // Номер ноги
    GPIO_Init(GPIOB, &PortB);
    
   RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE );  
    GPIO_InitTypeDef PortA;                     // Структура с необходимыми полями
    PortA.GPIO_Mode         = GPIO_Mode_Out_PP;    
    PortA.GPIO_Speed        = GPIO_Speed_10MHz;     // По сути это ток который сможет обеспечить вывод
    PortA.GPIO_Pin          = GPIO_Pin_8 | GPIO_Pin_5; // Номер ноги
    
    GPIO_Init(GPIOA, &PortA);
   
    Reset_Bit_PinB(&PortB);
    Reset_Bit_PinA(&PortA);
	if (RTC_Init() == 1) {
		// Если первая инициализация RTC устанавливаем начальную дату, например 22.09.2016 14:30:00
		RTC_DateTime.RTC_Date = 22;
		RTC_DateTime.RTC_Month = 9;
		RTC_DateTime.RTC_Year = 2016;

		RTC_DateTime.RTC_Hours = 13;
		RTC_DateTime.RTC_Minutes = 28;
		RTC_DateTime.RTC_Seconds = 00;

		//После инициализации требуется задержка. Без нее время не устанавливается.
		Delay2();
        Delay2();
        Delay2();
        Delay2();
        Delay2();
        Delay2();
		RTC_SetCounter(RTC_GetRTC_Counter(&RTC_DateTime));
    }
	while(1)
	{
        
      RTC_Counter = RTC_GetCounter();
      if(GPIO_ReadInputDataBit (GPIOA, GPIO_Pin_6) == 1)//	Если нажата кнопка PA0
		{
           RTC_Counter = RTC_GetCounter() + 0xC0;   
		}    
	  RTC_GetDateTime(RTC_Counter, &RTC_DateTime);
        
         //0
       /* GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_SET);
         GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_SET);
         Delay();
         GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_RESET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_SET);
         GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_SET);
         GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_RESET);
         Delay();
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);
         GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_RESET);
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_RESET);
         Delay();
          //1
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_SET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_RESET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_RESET);
         Delay();
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_RESET);
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_RESET);
         Delay();
          //2
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_9, Bit_SET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_9, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_RESET);
         Delay(); 
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_9, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_9, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_9, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_9, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_RESET);
         Delay();
          GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_9, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_9, Bit_RESET);
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_RESET);
         Delay();
         //3
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_RESET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_RESET);
         Delay();
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_RESET);
         Delay();
         //4
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_SET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_RESET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_RESET);
         Delay();
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_RESET);
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_RESET);
         Delay();
         //5
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_SET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_RESET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_RESET);
         Delay();
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_RESET);
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_RESET);
         Delay();
         //6
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_11, Bit_SET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_11, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_RESET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_11, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_11, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET);
         Delay();  
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_11, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_11, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_RESET);
         Delay();
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_11, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_11, Bit_RESET);
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_RESET);
         Delay();         
         //7
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_RESET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_RESET);
         Delay();
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET);
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_RESET);
         Delay();
          //8
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_SET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_RESET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_RESET); 
         Delay();
          GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_RESET);
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_RESET);
         Delay();
         //9
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_SET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_RESET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET);
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_RESET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_RESET);
         Delay();
          GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);
         GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_SET); 
         Delay();
         GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_RESET);
         GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_RESET);
         Delay();
     */
         Reset_Bit_PinB(&PortB);
         Reset_Bit_PinA(&PortA);
	  while (RTC_Counter == RTC_GetCounter())
       {
        Set_Bit_Pin(&PortB, &PortA, &RTC_DateTime);
        
       }
       copi(RTC_DateTime);
    }
 
}    
