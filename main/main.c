
#include <stdio.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"


// ==============================
// GPIO DEFINITIONS
// ==============================

#define IR_GPIO     GPIO_NUM_27
#define PIR_GPIO    GPIO_NUM_26


// ==============================
// EVENT TYPES
// ==============================

typedef enum {
    MOTION_DETECTED,
    OBSTACLE_DETECTED,
    REMOTE_COMMAND
} event_type_t;


// ==============================
// EVENT STRUCTURE
// ==============================

typedef struct {
    event_type_t type;
    int64_t timestamp_us;
} system_event_t;


// ==============================
// GLOBAL EVENT QUEUE
// ==============================

static QueueHandle_t event_queue;


// ==============================
// PIR INTERRUPT SERVICE ROUTINE
// ==============================

static void IRAM_ATTR pir_isr_handler(void *arg)
{
    system_event_t event;

    event.type = MOTION_DETECTED;
    event.timestamp_us = esp_timer_get_time();

    BaseType_t higher_priority_task_woken = pdFALSE;

    xQueueSendFromISR(
        event_queue,
        &event,
        &higher_priority_task_woken
    );

    if (higher_priority_task_woken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}


// ==============================
// EVENT MANAGER
// ==============================

static void event_manager_task(void *arg)
{
    system_event_t event;

    while (1) {

        if (xQueueReceive(
                event_queue,
                &event,
                portMAX_DELAY
            ) == pdTRUE) {

            int64_t timestamp_ms =
                event.timestamp_us / 1000;

            switch (event.type) {

                case MOTION_DETECTED:

                    printf(
                        "[%" PRId64 " ms] MOTION_DETECTED\n",
                        timestamp_ms
                    );

                    break;


                case OBSTACLE_DETECTED:

                    printf(
                        "[%" PRId64 " ms] OBSTACLE_DETECTED\n",
                        timestamp_ms
                    );

                    break;


                case REMOTE_COMMAND:

                    printf(
                        "[%" PRId64 " ms] REMOTE_COMMAND\n",
                        timestamp_ms
                    );

                    break;
            }
        }
    }
}


// ==============================
// IR SENSOR TASK
// ==============================

static void ir_sensor_task(void *arg)
{
    int previous_state = gpio_get_level(IR_GPIO);

    while (1) {

        int current_state =
            gpio_get_level(IR_GPIO);

        /*
         * Adjust this condition if your
         * P103 uses the opposite logic.
         */

        if (current_state == 0 &&
            previous_state == 1) {

            system_event_t event;

            event.type = OBSTACLE_DETECTED;
            event.timestamp_us =
                esp_timer_get_time();

            xQueueSend(
                event_queue,
                &event,
                0
            );
        }

        previous_state = current_state;

        vTaskDelay(
            pdMS_TO_TICKS(50)
        );
    }
}


// ==============================
// MAIN APPLICATION
// ==============================

void app_main(void)
{
    printf("\n");
    printf("====================================\n");
    printf(" ESP32 EDGE VISION STATION\n");
    printf(" Event Driven Firmware\n");
    printf("====================================\n");


    // ==============================
    // CREATE EVENT QUEUE
    // ==============================

    event_queue =
        xQueueCreate(
            10,
            sizeof(system_event_t)
        );


    if (event_queue == NULL) {

        printf("ERROR: Failed to create event queue\n");

        return;
    }


    // ==============================
    // CONFIGURE IR GPIO
    // ==============================

    gpio_config_t ir_config = {

        .pin_bit_mask =
            (1ULL << IR_GPIO),

        .mode =
            GPIO_MODE_INPUT,

        .pull_up_en =
            GPIO_PULLUP_DISABLE,

        .pull_down_en =
            GPIO_PULLDOWN_DISABLE,

        .intr_type =
            GPIO_INTR_DISABLE
    };

    gpio_config(&ir_config);


    // ==============================
    // CONFIGURE PIR GPIO
    // ==============================

    gpio_config_t pir_config = {

        .pin_bit_mask =
            (1ULL << PIR_GPIO),

        .mode =
            GPIO_MODE_INPUT,

        .pull_up_en =
            GPIO_PULLUP_DISABLE,

        .pull_down_en =
            GPIO_PULLDOWN_DISABLE,

        .intr_type =
            GPIO_INTR_POSEDGE
    };

    gpio_config(&pir_config);


    // ==============================
    // INSTALL GPIO ISR SERVICE
    // ==============================

    gpio_install_isr_service(0);


    // ==============================
    // CONNECT PIR TO ISR
    // ==============================

    gpio_isr_handler_add(
        PIR_GPIO,
        pir_isr_handler,
        NULL
    );


    // ==============================
    // START EVENT MANAGER
    // ==============================

    xTaskCreate(
        event_manager_task,
        "event_manager",
        4096,
        NULL,
        10,
        NULL
    );


    // ==============================
    // START IR SENSOR TASK
    // ==============================

    xTaskCreate(
        ir_sensor_task,
        "ir_sensor",
        4096,
        NULL,
        5,
        NULL
    );


    printf("System ready.\n");
    printf("IR  -> GPIO27\n");
    printf("PIR -> GPIO26 interrupt\n");
}