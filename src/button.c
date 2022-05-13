#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"
#include "button.h"

void _button_thread_func(void *argument);

#define BUTTON_SIGNAL 0x0001
static osThreadId_t        _button_thread_id;
static osEventFlagsId_t    _button_evt_id;
static osEventFlagsAttr_t _buttonEvent_attr =
{
    .name = "button"
};
static osThreadAttr_t _buttonTask_attr =
{
    .name = "button",
};

static uint8_t _was_pressed = 0;    // Sticky flag
static uint8_t _is_init = 0;

static GPIO_InitTypeDef GPIO_InitStruct; //copied from lab 9

void button_init(void)
{
    _was_pressed = 0;

    if (!_is_init)  // Guards against multiple initialisations
    {
    /* LAB2TODO:    Enable GPIOC clock                          */
    /*              Initialise PC7 with
                    - External interrupts on the rising edge
                    - Pulldown resistors enabled                */
    /* LAB2TODO:    Define structure to initialise GPIO */
    /* LAB2TODO:    Fill structure with correct parameters for
                    pin, mode, pull and speed */
    /* LAB2TODO:    Initialise GPIO using HAL function
    /* LAB2TODO:    Enable the EXTI9_5 interrupt                  */
    //ABOVE AS BELOW
    __HAL_RCC_GPIOC_CLK_ENABLE(); //copied from lab 9

    GPIO_InitStruct.Pin = GPIO_PIN_7; //copied from lab 9
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING; //copied from lab 9 |||||| added boeth rising and falling
    GPIO_InitStruct.Pull = GPIO_PULLDOWN; //copied from lab 9
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  //copied from lab 9

    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct); //copied from lab 9

    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0x0f, 0x0f); //copied from lab 9
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn); //copied from lab 9


    /*  TODO: Create an osThread for the button module using osThreadNew().
              - Store the id in _button_thread_id
              - The callback function is _button_thread_func
              - The function should take NULL input argument
              - The attributes should be set using the _buttonTask_attr structure */

  _button_thread_id = osThreadNew(_button_thread_func, NULL, &_buttonTask_attr);

    /*  TODO: Create an osEventFlag used for the button press using osEventFlagsNew().
              - Store the id in _button_evt_id
              - The attributes should be set using the _buttonEvent_attr structure. */
  _button_evt_id = osEventFlagsNew(&_buttonEvent_attr);

        _is_init = 1;
    }
}

void _button_thread_func(void *argument)
{
    UNUSED(argument); // Avoid compiler warning about unused variable
    while(1)
    {
        button_debounce();
    }
}

void button_debounce(void)
{
    typedef enum {
        BUTTON_LOW,
        BUTTON_HIGH,
    } BUTTON_STATE_T;

    static BUTTON_STATE_T state = BUTTON_LOW;  // Default state to LOW when thread is initialised

    switch (state) {
      case BUTTON_LOW:
        osEventFlagsWait(_button_evt_id, 0x00000001U, osFlagsWaitAny, osWaitForever/* wait forever for BUTTON_SIGNAL in _button_evt_id */);
        osDelay(30); // rising edge debounce
        if (pin is high) {
          _was_pressed = 1;
          state = BUTTON_HIGH;
        }
        // Clear additional pending events from bounce during delay
        osEventFlagsClear(_button_evt_id, /* clear BUTTON_SIGNAL in _button_evt_id */);
        break;

      case BUTTON_HIGH:
        osEventFlagsWait(/* wait for BUTTON_SIGNAL on _button_evt_id*/);
        osDelay(50); // falling edge debounce
        if (pin is low) {
          state = BUTTON_LOW;
        }
        // Clear additional pending events from bounce during delay
        osEventFlagsClear(/* clear BUTTON_SIGNAL in _button_evt_id */);
        break;
  }
}

void button_clear_pressed(void)
{
    _was_pressed = 0;
}

uint8_t button_get_pressed(void)
{
    return _was_pressed;
}

void button_set_pressed(void)
{
    _was_pressed = 1;
}

uint8_t button_pop_pressed(void)
{
    uint8_t was_pressed = _was_pressed;
    _was_pressed = 0;
    return was_pressed;
}

void button_isr(void)
{
    /* TODO: Set the osEventFlag 'BUTTON_SIGNAL' instead of the _was_pressed flag */
    osEventFlagsSet(set BUTTON_SIGNAL in _button_evt_id);
    //_was_pressed = 1; //TODO: Remove this naive un-debounced implementation
}

void EXTI9_5_IRQHandler(void)
{
    /* LAB2TODO: Call the button_isr() to indicate button was pressed */
    	button_isr();
    /* LAB2TODO: Call the HAL GPIO EXTI IRQ Handler and specify the GPIO pin to
    clear the pending hardware IRQ flag */
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
}

void button_deinit(void)    //Used for `make test'
{
    _is_init = 0;
    _was_pressed = 0;
}
