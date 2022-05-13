#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "potentiometer.h"

/*  TODO: Declare structures for ADC1
        TIP: Read the lab document and the HAL reference manual */


static uint8_t _is_init = 0;
static ADC_HandleTypeDef hadc1;
static ADC_ChannelConfTypeDef sConfigADC;

void pot_init(void)
{
    if (!_is_init)
    {
	    /* TODO: Initialise the ADC peripheral for the config:
                ADC1 instance,DONE
                div2 prescaler,DONE
                12-bit resolution, DONE
                left data aligned, DONE
                continuous conversion mode.DONE
        Note:   Config parameters of the ADC_InitTypeDef not mentioned above
                are not required in this lab            */
	hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.DataAlign = ADC_DATAALIGN_LEFT;
	hadc1.Init.ContinuousConvMode = ENABLE;
	
    /* TODO: Configure the ADC channel for:
                channel 0,DONE
                480 cycle sample time.  DONE                */	
	sConfigADC.Channel = ADC_CHANNEL_0;
    sConfigADC.SamplingTime = ADC_SAMPLETIME_480CYCLES;			
	
	
	
        /* TODO: Initialise the ADC                     */ 
	HAL_ADC_Init(&hadc1);
        /* TODO: Configure the ADC channel              */
	HAL_ADC_ConfigChannel(&hadc1, &sConfigADC);	
        /* TODO: Start the ADC                          */
	HAL_ADC_Start(&hadc1);
        
        _is_init = 1;
    }
}

void pot_deinit(void)
{
    _is_init = 0;
}

uint16_t pot_get_value(void)
{
    uint16_t result = 0;
    /* TODO: Poll the ADC for a conversion   DONE       */
	HAL_ADC_PollForConversion(&hadc1, 0xff);
    /* TODO: Get and return the 12-bit result  DONE     */
	result = HAL_ADC_GetValue(&hadc1);
   
    return result; 
}
