#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define TEST_GPIO GPIO_NUM_2

void app_main(void)
{
    gpio_reset_pin(TEST_GPIO);
    gpio_set_direction(TEST_GPIO, GPIO_MODE_OUTPUT);

    printf("\n");
    printf("=====================================\n");
    printf(" ESP32 Edge Vision Station\n");
    printf(" Day 2 - GPIO Hardware Bring-Up\n");
    printf("=====================================\n");

    while (1)
    {
        gpio_set_level(TEST_GPIO, 1);
        printf("GPIO 2 -> HIGH\n");

        vTaskDelay(pdMS_TO_TICKS(1000));

        gpio_set_level(TEST_GPIO, 0);
        printf("GPIO 2 -> LOW\n");

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}