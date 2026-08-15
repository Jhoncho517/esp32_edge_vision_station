#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define IR_PIN GPIO_NUM_27

void app_main(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << IR_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    gpio_config(&io_conf);

    printf("=================================\n");
    printf(" ESP32 Edge Vision Station\n");
    printf(" IR Sensor Test\n");
    printf("=================================\n");

    while (1)
    {
        int ir_state = gpio_get_level(IR_PIN);

        printf("GPIO27 = %d\n", ir_state);

        if (ir_state == 0)
        {
            printf("OBJECT DETECTED\n");
        }
        else
        {
            printf("NO OBJECT\n");
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}