#include "stm32f4xx_hal.h"

static GPIO_InitTypeDef  GPIO_Init_PA0;
static GPIO_InitTypeDef  GPIO_Init_tim4;

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
	/* Timer 4: Initialise the RGB LEDs on PB6, PB8, PB9 */
    if(htim->Instance == TIM4)
    {
        /* Enable peripheral clocks */
        __HAL_RCC_TIM4_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        /* Configure PB6, PC8 & PC9 in AF pushpull mode with TIM4 alternate function*/
        GPIO_InitTypeDef  GPIO_InitStructure;
        GPIO_InitStructure.Pin = GPIO_PIN_6|GPIO_PIN_8|GPIO_PIN_9;
        GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStructure.Pull = GPIO_PULLUP;
        GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStructure.Alternate = GPIO_AF2_TIM4;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
    }
   	/* Timer 2: Initialise the onboard LED for heartbeat on PA5 */
    else if(htim->Instance == TIM2)
    {
        /* Enable peripheral clocks */
        __HAL_RCC_TIM2_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        /* Configure PA5 in AF pushpull mode with TIM2 alternate function */
        GPIO_InitTypeDef  GPIO_InitStructure;
        GPIO_InitStructure.Pin = GPIO_PIN_5;
        GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStructure.Pull = GPIO_PULLUP;
        GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStructure.Alternate = GPIO_AF1_TIM2;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
    }
}

void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
    if(hadc->Instance == ADC1)
    {
        /* LAB2TODO: Enable ADC1 clock */
        /* LAB2TODO: Enable GPIOA clock */
        /* LAB2TODO: Configure PA0 in analog mode, no pullup */

        __HAL_RCC_ADC1_CLK_ENABLE();
/* TODO: Enable GPIOA clock DONE*/
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /* TODO: Configure PA0 in analog mode, no pullup DONE*/

    GPIO_Init_PA0.Pin = GPIO_PIN_0;
    GPIO_Init_PA0.Mode = GPIO_MODE_ANALOG;  	//analog mode for use with adc
    GPIO_Init_PA0.Pull = GPIO_NOPULL;  			//Disable pull up resitors
    //GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(GPIOA, &GPIO_Init_PA0);
    }
}
