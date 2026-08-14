#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    printf("\n");
    printf("====================================\n");
    printf(" ESP32 Edge Vision Station\n");
    printf(" Firmware starting...\n");
    printf("====================================\n");

    while (1)
    {
        printf("System alive\n");

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}