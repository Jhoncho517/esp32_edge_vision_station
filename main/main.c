#include <stdio.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"
#include "esp_timer.h"

#include "driver/rmt_rx.h"
#include "esp_err.h"


// ============================================================
// GPIO DEFINITIONS
// ============================================================

#define IR_GPIO       GPIO_NUM_27
#define PIR_GPIO      GPIO_NUM_26
#define REMOTE_GPIO   GPIO_NUM_25

#define RMT_RESOLUTION_HZ 1000000
#define RMT_BUFFER_SIZE   256


// ============================================================
// EVENT TYPES
// ============================================================

typedef enum {
    MOTION_DETECTED,
    OBSTACLE_DETECTED,
    REMOTE_COMMAND
} event_type_t;


// ============================================================
// EVENT STRUCTURE
// ============================================================

typedef struct {
    event_type_t type;
    int64_t timestamp_us;
} system_event_t;


// ============================================================
// GLOBAL EVENT QUEUE
// ============================================================

static QueueHandle_t event_queue;


// ============================================================
// PIR INTERRUPT SERVICE ROUTINE
// ============================================================

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


// ============================================================
// EVENT MANAGER
// ============================================================

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


// ============================================================
// IR OBSTACLE SENSOR TASK
// ============================================================

static void ir_sensor_task(void *arg)
{
    int previous_state =
        gpio_get_level(IR_GPIO);

    while (1) {

        int current_state =
            gpio_get_level(IR_GPIO);

        /*
         * P103 obstacle sensor:
         *
         * LOW  = obstacle detected
         * HIGH = no obstacle
         *
         * If your module behaves opposite,
         * reverse this condition.
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

        previous_state =
            current_state;

        vTaskDelay(
            pdMS_TO_TICKS(50)
        );
    }
}


// ============================================================
// RMT REMOTE RECEIVER
// ============================================================

static rmt_channel_handle_t remote_rmt_channel = NULL;

static bool IRAM_ATTR remote_rmt_callback(
    rmt_channel_handle_t channel,
    const rmt_rx_done_event_data_t *edata,
    void *user_data)
{
    BaseType_t higher_priority_task_woken = pdFALSE;

    // Notify the task that an IR frame was received
    vTaskNotifyGiveFromISR(
        (TaskHandle_t)user_data,
        &higher_priority_task_woken
    );

    return higher_priority_task_woken == pdTRUE;
}


// ============================================================
// REMOTE RECEIVER TASK
// ============================================================

static void remote_receiver_task(void *arg)
{
    rmt_symbol_word_t symbols[RMT_BUFFER_SIZE];

    rmt_receive_config_t receive_config = {
        .signal_range_min_ns = 1000,
        .signal_range_max_ns = 12000000,
    };

    rmt_rx_event_callbacks_t callbacks = {
        .on_recv_done = remote_rmt_callback,
    };

    ESP_ERROR_CHECK(
        rmt_rx_register_event_callbacks(
            remote_rmt_channel,
            &callbacks,
            xTaskGetCurrentTaskHandle()
        )
    );

    ESP_ERROR_CHECK(
        rmt_enable(remote_rmt_channel)
    );

    printf("Remote RMT receiver started.\n");

    while (1) {

        esp_err_t err = rmt_receive(
            remote_rmt_channel,
            symbols,
            sizeof(symbols),
            &receive_config
        );

        if (err != ESP_OK) {

            printf(
                "RMT receive error: %s\n",
                esp_err_to_name(err)
            );

            vTaskDelay(
                pdMS_TO_TICKS(100)
            );

            continue;
        }

        /*
         * Wait for an IR frame to complete.
         *
         * Unlike the old polling implementation,
         * the CPU is NOT continuously checking GPIO25.
         */

        ulTaskNotifyTake(
            pdTRUE,
            portMAX_DELAY
        );

        /*
         * At this point the RMT peripheral has captured
         * an infrared waveform.
         *
         * For now we treat the captured frame as a
         * REMOTE_COMMAND event.
         */

        system_event_t event;

        event.type =
            REMOTE_COMMAND;

        event.timestamp_us =
            esp_timer_get_time();

        xQueueSend(
            event_queue,
            &event,
            0
        );
    }
}


// ============================================================
// MAIN APPLICATION
// ============================================================

void app_main(void)
{
    printf("\n");
    printf("====================================\n");
    printf(" ESP32 EDGE VISION STATION\n");
    printf(" Day 3 - Event Driven Firmware\n");
    printf("====================================\n");


    // ========================================================
    // CREATE EVENT QUEUE
    // ========================================================

    event_queue =
        xQueueCreate(
            20,
            sizeof(system_event_t)
        );

    if (event_queue == NULL) {

        printf(
            "ERROR: Failed to create event queue\n"
        );

        return;
    }


    // ========================================================
    // CONFIGURE IR OBSTACLE SENSOR
    // ========================================================

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


    // ========================================================
    // CONFIGURE PIR
    // ========================================================

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


    // ========================================================
    // CONFIGURE REMOTE IR RECEIVER
    // ========================================================

    gpio_config_t remote_config = {

        .pin_bit_mask =
            (1ULL << REMOTE_GPIO),

        .mode =
            GPIO_MODE_INPUT,

        .pull_up_en =
            GPIO_PULLUP_DISABLE,

        .pull_down_en =
            GPIO_PULLDOWN_DISABLE,

        .intr_type =
            GPIO_INTR_DISABLE
    };

    gpio_config(&remote_config);


    // ============================================================
    // CONFIGURE RMT REMOTE RECEIVER
    // ============================================================

    rmt_rx_channel_config_t remote_rmt_config = {
        .gpio_num = REMOTE_GPIO,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = RMT_RESOLUTION_HZ,
         .mem_block_symbols = RMT_BUFFER_SIZE,
        };

    esp_err_t rmt_err = rmt_new_rx_channel( &remote_rmt_config, &remote_rmt_channel);
    
    if (rmt_err != ESP_OK) {
        printf(
        "ERROR: Failed to create RMT RX channel: %s\n",
        esp_err_to_name(rmt_err)
    );
    return;
    }

    // ========================================================
    // INSTALL GPIO ISR SERVICE
    // ========================================================

    esp_err_t isr_result =
        gpio_install_isr_service(0);

    if (isr_result != ESP_OK &&
        isr_result != ESP_ERR_INVALID_STATE) {

        printf(
            "ERROR: Failed to install ISR service\n"
        );

        return;
    }


    // ========================================================
    // CONNECT PIR TO INTERRUPT
    // ========================================================

    gpio_isr_handler_add(
        PIR_GPIO,
        pir_isr_handler,
        NULL
    );


    // ========================================================
    // START EVENT MANAGER
    // ========================================================

    xTaskCreate(
        event_manager_task,
        "event_manager",
        4096,
        NULL,
        10,
        NULL
    );


    // ========================================================
    // START IR SENSOR TASK
    // ========================================================

    xTaskCreate(
        ir_sensor_task,
        "ir_sensor",
        4096,
        NULL,
        5,
        NULL
    );


    // ========================================================
    // START REMOTE RECEIVER TASK
    // ========================================================

    xTaskCreate(
        remote_receiver_task,
        "remote_receiver",
        4096,
        NULL,
        5,
        NULL
    );


    // ========================================================
    // SYSTEM READY
    // ========================================================

    printf("\n");
    printf("System ready.\n");
    printf("IR     -> GPIO27\n");
    printf("PIR    -> GPIO26 interrupt\n");
    printf("REMOTE -> GPIO25\n");
    printf("\n");

    printf("Waiting for events...\n");
}