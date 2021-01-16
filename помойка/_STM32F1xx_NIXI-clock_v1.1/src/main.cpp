/*
    @brief NIXI-v1.1/13 21.02.2014
    @author: Ht3h5793
*/


#include "ch.h"
#include "hal.h"

static uint32_t i;

// PWM
static PWMConfig pwmcfg = {
    20000,	// 2KHz PWM clock frequency
    10,		// PWM period 20 milliseconds
	NULL,		// no callback
	NULL,		// channel configuration set dynamicall in servoInit()
    0
};


// RTC
#include "chrtclib.h"
RTCTime timespec;
RTCAlarm alarmspec;
// Структура для календаря
tm stRTC;


// ADC клавиатуры
uint32_t KeyValue = 3300;
#define ADC_GRP1_NUM_CHANNELS   1
#define ADC_GRP1_BUF_DEPTH      4
static adcsample_t samples1[ADC_GRP1_NUM_CHANNELS * ADC_GRP1_BUF_DEPTH];
/*
 * ADC conversion group.
 * Mode:        Linear buffer, 8 samples of 1 channel, SW triggered.
 * Channels:    IN10.
 */
static const ADCConversionGroup adcgrpcfg1 = {
  FALSE,
  ADC_GRP1_NUM_CHANNELS,
  NULL,
  NULL, //adcerrorcallback,
  0, 0,    /* CR1, CR2 */
  ADC_SMPR1_SMP_AN10(ADC_SAMPLE_7P5), //  7.5 cycles
  0,                            /* SMPR2 */
  ADC_SQR1_NUM_CH(ADC_GRP1_NUM_CHANNELS),
  0,                            /* SQR2 */
  ADC_SQR3_SQ1_N(ADC_CHANNEL_IN0)
};


#include "i2c_pns.h"
#include "tmp75.h"
  /* temperature value */
static  uint8_t _tempLM75 = 0;
/* Temperature polling thread */
static WORKING_AREA(PollTmp75ThreadWA, 128); //256
static msg_t PollTmp75Thread(void *arg) {
    chRegSetThreadName("PollTmp75");
    (void)arg;
    while (TRUE) {
        /* Call reading function */
        request_temperature( &_tempLM75);
        chThdSleepMilliseconds(200);    /*chThdSleepMilliseconds(rand() & 31);*/
    }
}

uint8_t blinckBit = TRUE; // Лампочка мигания
typedef enum {
    TIME_MIN_SEC = 0,
    TIME_HOUR_MIN,
    MODE_TEMP,
    
}MODE;
uint8_t mode = TIME_MIN_SEC;
static WORKING_AREA(BlinkThreadWA, 16);
static msg_t BlinkThread(void *arg) {
    chRegSetThreadName(" ");
    (void)arg;
    while (TRUE) {
        blinckBit = ~blinckBit;
        chThdSleepMilliseconds(500);
    }
}


#define  MAX_SIZE_STRING  2
/** массив с цифрами HEX системы в ASCII **/
const static unsigned char harDEC[] = "0123456789";
//const static unsigned char harHEX[] = "0123456789ABCDEF";
uint8_t _valTo_DECStr(uint8_t *string, uint32_t value) 
{
    uint8_t ms[MAX_SIZE_STRING];
    uint8_t size, i;
    uint32_t temp;

    i = MAX_SIZE_STRING;
    while(i--)
    {
        ms[i] = 0; // Чистим все предварительно
        temp = (value % 10);
        ms[i] = temp;
        value /= 10;
    }
    
    size = 0;
    // Незначущие нули
     for (i = 0; i < MAX_SIZE_STRING; i++)
    {
         //if (ms[i] != 0)
         {
             // Формирование строки 
             string[size] = harDEC[ms[i]];
             size++;
         }
    }
    //string[size] = '\0'; // Завершаем строку на всякий случай
      
    return size;
}


#define NIXI_SET_1  11 // ALL A
#define NIXI_SET_2  2
#define NIXI_SET_3  4
#define NIXI_SET_4  3

#define NIXI_0   13 //B
#define NIXI_1   14 //B
#define NIXI_2   12 //B
#define NIXI_3   10 //B
#define NIXI_4   6 //A
#define NIXI_5   0 //B
#define NIXI_6   1 //B
#define NIXI_7   7  //A
#define NIXI_8   2  //B
#define NIXI_9   11  //B
#define NIXI_DP  15  //B
#define NIXI_PS  8  //A

uint8_t nixiSel = 0;
uint8_t bufL[2+2]; // под точки зарезервируем
uint8_t bufR[2+2];
static struct VirtualTimer vt;
void timer_handler(void *arg)
{
    
    volatile unsigned long n;
    uint8_t c = 0;
    
    palClearPort(GPIOA, (1<<NIXI_SET_1) | (1<<NIXI_SET_2)
                      | (1<<NIXI_SET_3) | (1<<NIXI_SET_4));
    // Задержка для транзисторов
	for (n = 0; n < 100; n++){};
    
    switch(nixiSel)
    {
        case 0: 
            palSetPad(GPIOA, NIXI_SET_1);
            c = bufL[0];
            nixiSel = 1;
            break;
        case 1: 
            palSetPad(GPIOA, NIXI_SET_2);
            c = bufL[1];
            nixiSel = 2;
            break;
        case 2: 
            palSetPad(GPIOA, NIXI_SET_3);
            c = bufR[0];
            nixiSel = 3;
            break;
        case 3: 
            palSetPad(GPIOA, NIXI_SET_4);
            c = bufR[1];
            nixiSel = 0;
            break;
        
        default: nixiSel = 0; break;
    }
    
    palClearPort(GPIOB, (1<<NIXI_0) | (1<<NIXI_1) | (1<<NIXI_2)
                      | (1<<NIXI_3) | (1<<NIXI_5) | (1<<NIXI_6)
                      | (1<<NIXI_8) | (1<<NIXI_9) | (1<<NIXI_DP));
     
    palClearPort(GPIOA, (1<<NIXI_4) | (1<<NIXI_7));
    
    // Задержка для транзисторов
	for (n = 0; n < 100; n++){};

    switch(c)
    {
        case '0': palSetPad(GPIOB, NIXI_0); break;
        case '1': palSetPad(GPIOB, NIXI_1); break;
        case '2': palSetPad(GPIOB, NIXI_2); break;
        case '3': palSetPad(GPIOB, NIXI_3); break;
        case '4': palSetPad(GPIOA, NIXI_4); break;
        case '5': palSetPad(GPIOB, NIXI_5); break;
        case '6': palSetPad(GPIOB, NIXI_6); break;
        case '7': palSetPad(GPIOA, NIXI_7); break;
        case '8': palSetPad(GPIOB, NIXI_8); break;
        case '9': palSetPad(GPIOB, NIXI_9); break;
        case '.': palSetPad(GPIOB, NIXI_DP); break;
        default: palSetPad(GPIOB, NIXI_0); break; // пусть нолик будет
    }
    
    if (TRUE == blinckBit)
    {
        palSetPad(GPIOA, NIXI_PS);
    } else {
        palClearPad(GPIOA, NIXI_PS);
    }
    
    // Переиничивание таймера (возможно на другой период)
    chVTSetI(&vt, MS2ST( 5 ), timer_handler, 0);
}


/*
 * Application entry point.
 */
int main(void) {

    /*
    * System initializations.
    * - HAL initialization, this also initializes the configured device drivers
    *   and performs the board-specific initializations.
    * - Kernel initialization, the main() function becomes a thread and the
    *   RTOS is active.
    */
    halInit();
    chSysInit();

    // Настройка портов-катодов
    palSetPadMode(GPIOB, NIXI_0, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOB, NIXI_1, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOB, NIXI_2, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOB, NIXI_3, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOA, NIXI_4, PAL_MODE_OUTPUT_PUSHPULL); //
    palSetPadMode(GPIOB, NIXI_5, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOB, NIXI_6, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOA, NIXI_7, PAL_MODE_OUTPUT_PUSHPULL); //
    palSetPadMode(GPIOB, NIXI_8, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOB, NIXI_9, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOB, NIXI_DP, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOA, NIXI_PS, PAL_MODE_OUTPUT_PUSHPULL); //
    
    palSetPadMode(GPIOA, NIXI_SET_1, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOA, NIXI_SET_2, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOA, NIXI_SET_3, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOA, NIXI_SET_4, PAL_MODE_OUTPUT_PUSHPULL);
    
    chThdSleepMilliseconds(50); // Ждем микросхему термометра
    I2CInit_pns();
    /* Create temperature thread */
    chThdCreateStatic(PollTmp75ThreadWA,
          sizeof(PollTmp75ThreadWA),
          NORMALPRIO,
          PollTmp75Thread,
          NULL);

    /* Задача мигания */
    chThdCreateStatic(BlinkThreadWA,
          sizeof(BlinkThreadWA),
          NORMALPRIO,
          BlinkThread,
          NULL);
          
    // ADC
    palSetGroupMode(GPIOA, PAL_PORT_BIT(0) | PAL_PORT_BIT(1),
                  0, PAL_MODE_INPUT_ANALOG);
    /*
    * Activates the ADC1 driver and the temperature sensor.
    */
    adcStart(&ADCD1, NULL);
    
    //rtcGetTime(&RTCD1, &timespec);
    //alarmspec.tv_sec = timespec.tv_sec + 30;
    //rtcSetAlarm(&RTCD1, 0, &alarmspec);
    rtcGetTimeTm(&RTCD1, &stRTC);
    stRTC.tm_sec  =   30;
    stRTC.tm_min  =   59;
    stRTC.tm_hour =   12;
    rtcSetTimeTm(&RTCD1, &stRTC);

    // Программный таймер, 6ms
    chVTSetI(&vt, MS2ST( 5 ), timer_handler, 0);

    // Настойка вывода PWM
    palSetPadMode(GPIOA, 1 , PAL_MODE_STM32_ALTERNATE_PUSHPULL); 
    PWMChannelConfig chcfg = { PWM_OUTPUT_ACTIVE_HIGH, NULL };
	//pwmcfg.channels[0] = chcfg;
    pwmcfg.channels[1] = chcfg;
    //pwmcfg.channels[2] = chcfg;
    //pwmcfg.channels[3] = chcfg;
    /* start the PWM unit */
	pwmStart( &PWMD2, &pwmcfg);
    pwmEnableChannel( &PWMD2, 1, 5 ); // Запуск преобразователя, 4-ре - коефициент заполнения
   
    while(TRUE != FALSE)
    {

        // Одновременно это будет задержкой
        adcConvert(&ADCD1, &adcgrpcfg1, samples1, ADC_GRP1_BUF_DEPTH);
        
        KeyValue = 0;
        for (i=0; i < ADC_GRP1_BUF_DEPTH; i++)
        {
            KeyValue += samples1[i];
        }
        KeyValue = KeyValue / ADC_GRP1_BUF_DEPTH;
                          
        if ( KeyValue > 100 )
        {
            if ((KeyValue > 2400) && (KeyValue < 2700)) //R
            {
                rtcGetTimeTm(&RTCD1, &stRTC);
                if (TIME_MIN_SEC == mode)
                    stRTC.tm_sec++;
                else
                    stRTC.tm_min++;
                rtcSetTimeTm(&RTCD1, &stRTC);
            }
            if ((KeyValue > 1800) && (KeyValue < 2100)) // L
            {
                rtcGetTimeTm(&RTCD1, &stRTC);
                if (TIME_MIN_SEC == mode)
                    stRTC.tm_min++;
                else
                    stRTC.tm_hour++;
                rtcSetTimeTm(&RTCD1, &stRTC);
            }
        }
        else // M
        {
            switch(mode)
            {
                case TIME_MIN_SEC:  mode = TIME_HOUR_MIN; break;
                case TIME_HOUR_MIN: mode = MODE_TEMP;     break;
                case MODE_TEMP:     mode = TIME_MIN_SEC;  break;
                default : mode = TIME_MIN_SEC; break;
            }                  
        }

      
        rtcGetTimeTm(&RTCD1, &stRTC);
        switch(mode) // Перерисовка 
        {
            case TIME_MIN_SEC:  
                _valTo_DECStr( &bufL[0], stRTC.tm_min);
                _valTo_DECStr( &bufR[0], stRTC.tm_sec); 
                break;
            
            case TIME_HOUR_MIN:
                _valTo_DECStr( &bufL[0], stRTC.tm_hour);
                _valTo_DECStr( &bufR[0], stRTC.tm_min);   
                break;
            
            case MODE_TEMP:
                _valTo_DECStr( &bufL[0], 0);
                _valTo_DECStr( &bufR[0], _tempLM75);
                break;
        }         

        chThdSleepMilliseconds(250);
    };
};
