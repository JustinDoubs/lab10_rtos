#include "stm32f4xx_hal.h"
#include "led_rgb.h"

/* TODO: Declare structures for RGB LEDs on Timer 4 */

static uint8_t _is_init = 0;
static TIM_HandleTypeDef htim4;
static TIM_OC_InitTypeDef sConfigPWM;


void led_rgb_init(void)
{
    /* TODO:    Cut/paste the code between the  '---' lines to move
                the GPIO Initialisation in HAL_TIM_PWM_MspInit().
                Then, complete the TODO's.
        ----------------------------------------------------------  */
//DONE code was removed from here and is pasted in the HAL_TIM_PWM_MspInit function in file : stm32f4xx_hal_msp.c
    /*  ----------------------------------------------------------  */

    if (!_is_init) // Guards against multiple initialisations
    {
        /* TODO:    Configure Timer4 to generate three PWM outputs on
                    PB6(TIM4_CH1), PB8(TIM4_CH3), PB9(TIM4_CH4) with $DO THIS$
                    - Prescaler of 1 DONE
                    - Counting up DONE
                    - Period of 65535 (maximum 16 bit value) DONE
                    - Clock division of one DONE*/
      
      htim4.Instance = TIM4;
      htim4.Init.Prescaler = 1;
      htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
      htim4.Init.Period = 65535;
      htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

        /* TODO:    Configure the timers for a mode and polarity suitable
                    for common anode LED. i.e. '0' is a 'HIGH' signal.
                    Set the default duty cycle to 0%                    */
      
      sConfigPWM.OCMode = TIM_OCMODE_PWM1; //not sure how to pick these
      sConfigPWM.Pulse = 0;
	  sConfigPWM.OCFastMode = TIM_OCFAST_DISABLE;
      sConfigPWM.OCPolarity = TIM_OCPOLARITY_LOW;

        /* TODO:    Initialise timer 4 in PWM mode                      */
      HAL_TIM_PWM_Init(&htim4);
        /* TODO:    Configure timer 4 channels 1, 3 and 4 in PWM mode   */
      HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigPWM, TIM_CHANNEL_1);
	  HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigPWM, TIM_CHANNEL_3);
	  HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigPWM, TIM_CHANNEL_4);
        /* TODO:    Start timer 4 channels 1, 3 and 4 in PWM mode       */
      HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
	  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
	  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
        _is_init = 1;
    }
    // Set colours to off by default
    led_rgb_set_red(0);
    led_rgb_set_green(0);
    led_rgb_set_blue(0);
}

void led_rgb_deinit(void)
{
    _is_init = 0;
}

uint16_t led_rgb_get_red(void)
{
    /* TODO: Get and return the PWM duty cycle of channel 1 */
    return __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1); /* Tip: Don't forget to change this return value! */
}
uint16_t led_rgb_get_green(void)
{
    /* TODO: Get and return the PWM duty cycle of channel 3 */
    return __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_3); /* Tip: Don't forget to change this return value! */
}
uint16_t led_rgb_get_blue(void)
{
    /* TODO: Get and return the PWM duty cycle of channel 4 */
    return __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_4); /* Tip: Don't forget to change this return value! */
}
void led_rgb_set_red(uint16_t R)
{
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, R);
}
void led_rgb_set_green(uint16_t G)
{
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, G);
}
void led_rgb_set_blue(uint16_t B)
{
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, B);
}
