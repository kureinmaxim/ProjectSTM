/* Simple LED task demo, using timed delays:
 *
 * The LED on PC13 is toggled in task1.
 */

// !!! in FreeRTOSConfig.h add string
// #define INCLUDE_uxTaskGetStackHighWaterMark 1
// для контроля заполнения стека, если осталось
// менее 10 слов (8*10=80 байт), то часто мигает светодиод С13

#include "FreeRTOS.h"
#include "task.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

extern void vApplicationStackOverflowHook(
	xTaskHandle *pxTask,
	signed portCHAR *pcTaskName);

void
vApplicationStackOverflowHook(
  xTaskHandle *pxTask __attribute((unused)),
  signed portCHAR *pcTaskName __attribute((unused))
)
{
  for(;;);	// Loop forever here..
}

static void
task1(void *args __attribute((unused))) {

  for (;;)
   {
 	 gpio_toggle(GPIOC,GPIO13);
	 vTaskDelay(pdMS_TO_TICKS(500));
 // Получаем оставшийся запас стека (в словах)
     UBaseType_t stackRemaining = uxTaskGetStackHighWaterMark(NULL);
 // Можно вывести в UART или отладчик
 // printf("task1: Остаток стека: %lu слов\n", stackRemaining);
     if (stackRemaining < 10)
      {
        while (1)
         {
           gpio_toggle(GPIOC, GPIO13);
           vTaskDelay(pdMS_TO_TICKS(50));
         }
      }
   }
 }

static void
task2(void *args __attribute((unused))) {

	for (;;)
     {
		gpio_toggle(GPIOC,GPIO15);
		vTaskDelay(pdMS_TO_TICKS(10));
	 }
}

int
main(void) {

	rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);

	rcc_periph_clock_enable(RCC_GPIOC);
	gpio_set_mode(GPIOC,GPIO_MODE_OUTPUT_2_MHZ,GPIO_CNF_OUTPUT_PUSHPULL,GPIO13);
    gpio_set_mode(GPIOC,GPIO_MODE_OUTPUT_2_MHZ,GPIO_CNF_OUTPUT_PUSHPULL,GPIO15);

	xTaskCreate(task1,"LED",100,NULL,configMAX_PRIORITIES-1,NULL);
    xTaskCreate(task2,"PinOut",50,NULL,configMAX_PRIORITIES-2,NULL);
	vTaskStartScheduler();

	for (;;);
	return 0;
}

// End
