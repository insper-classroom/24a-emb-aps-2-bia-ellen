/*
 * LED blink with FreeRTOS
 */
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include <string.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "hc06.h"

const int BTN_PIN_ENTER = 17;
QueueHandle_t xQueueBTN;

void btn_callback(uint gpio, uint32_t events){
    if(gpio==BTN_PIN_ENTER){
        char data[4] = "ENT";
        xQueueSendFromISR(xQueueBTN, &data, 0);
    }

}

void hc06_task(void *p) {
    uart_init(HC06_UART_ID, HC06_BAUD_RATE);
    gpio_set_function(HC06_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(HC06_RX_PIN, GPIO_FUNC_UART);
    hc06_init("bia_ellen", "5678");

    char data[4];

    while (true) {
        if (xQueueReceive(xQueueBTN, &data, 100)) {
            uart_puts(HC06_UART_ID, data);
            vTaskDelay(pdMS_TO_TICKS(50));
        } else{
            uart_puts(HC06_UART_ID, "NAN");
            vTaskDelay(pdMS_TO_TICKS(50));
        }
        
    }
}

void game_task(void *p){
    gpio_init(BTN_PIN_ENTER);
    gpio_set_dir(BTN_PIN_ENTER, GPIO_IN);
    gpio_pull_up(BTN_PIN_ENTER);

    gpio_set_irq_enabled_with_callback(BTN_PIN_ENTER, GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    while(true){

    }
}

int main() {
    stdio_init_all();

    printf("Start bluetooth task\n");

    xQueueBTN = xQueueCreate(64, 4*sizeof(char));

    xTaskCreate(game_task, "Game Task", 4096,  NULL, 1, NULL);
    xTaskCreate(hc06_task, "UART_Task 1", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
