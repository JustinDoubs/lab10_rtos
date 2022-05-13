#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"
#include "button.h"
#include "led_rgb.h"
#include "light.h"
#include "potentiometer.h"
#include "dimmer_task.h"

static osThreadId_t _dimmer_task_thread_id;
const osThreadAttr_t _dimmerTask_attr =
{
    .name = "dimmer",
    .priority = osPriorityLow //Set initial thread priority
};

static void _dimmer_task_update(void *argument);

static uint8_t _is_running = 0;
static uint8_t _is_init = 0;

void dimmer_task_init(void)
{
    if (!_is_init)
    {
        /*  TODO: Create an osThread using osThreadNew().
              - Store the id in _dimmer_task_thread_id
              - The callback function is _dimmer_task_update
              - The function should take NULL input argument
              - The attributes should be set using the _dimmerTask_attr structure

        TIP:    _dimmer_task_thread_id is defined for you at the top of this
                c file. Don't define it twice!)                 */
      _dimmer_task_thread_id = osThreadNew(_dimmer_task_update, NULL, &_dimmerTask_attr);

        _is_running = 1;
        _is_init = 1;
    }
}

void dimmer_task_resume(void)
{
    if (!_is_running)
    {
        /* TODO: Resume the dimmer thread */
        osThreadResume(_dimmer_task_thread_id);
        _is_running = 1;
    }
}

void dimmer_task_stop(void)
{
    if (_is_running)
    {
        /* TODO: Suspend the dimmer thread */
      osThreadSuspend(_dimmer_task_thread_id);
      _is_running = 0;
    }
}

uint8_t dimmer_task_is_running(void)
{
    return _is_running;
}

void _dimmer_task_update(void *argument)
{
    UNUSED(argument);
    while(1)
    {
        // Read potentiometer value and set hue
        light_set_hue(pot_get_value());
        // Read button and toggle colour
        if (button_pop_pressed())
        {
            // Toggle between 0% and 100% saturation
            light_set_saturation(LIGHT_MAX_SATURATION - light_get_saturation());
        }
        // Update LED brightness
        led_rgb_set_red(light_get_red());
        led_rgb_set_green(light_get_green());
        led_rgb_set_blue(light_get_blue());
    }
}

void dimmer_task_deinit(void)   //Used by `make test'
{
    _is_init = 0;
    _is_running = 0;
}
