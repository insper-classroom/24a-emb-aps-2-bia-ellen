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
#include "hardware/adc.h"

const int BTN_PIN_ENTER = 12;
const int BTN_PIN_ESC = 13;
const int GARRA_DIREITA = 14;
const int GARRA_ESQUERDA = 15;
const int BTN_PRESSION = 26;
const int ANL_X = 27;
const int ANL_Y = 28; 
const int VIBRA = 17;
const int LED_BLUETOOTH = 10;

QueueHandle_t xQueueBTN;
QueueHandle_t xQueueMouse;
QueueHandle_t xQueueBall;

bool timer_0_callback(repeating_timer_t *rt) {
    int led_status = gpio_get(LED_BLUETOOTH);
    gpio_put(LED_BLUETOOTH, !led_status);
    return true; // keep repeating
}

void btn_callback(uint gpio, uint32_t events){
    if(gpio==BTN_PIN_ENTER){
        char data[5] = "2ENT";
        xQueueSendFromISR(xQueueBTN, &data, 0);
    }
    if(gpio==BTN_PIN_ESC){
        char data[5] = "2ESC";
        xQueueSendFromISR(xQueueBTN, &data, 0);
    }
    if(gpio==GARRA_DIREITA){
        if(events==GPIO_IRQ_EDGE_FALL){
            char data[5] = "2GDD"; // Garra Direita Desce
            xQueueSendFromISR(xQueueBTN, &data, 0);
        } if(events==GPIO_IRQ_EDGE_RISE){
            char data[5] = "2GDS"; // Garra Direita Sobe
            xQueueSendFromISR(xQueueBTN, &data, 0);
        }
    }
    if(gpio==GARRA_ESQUERDA){
        if(events==GPIO_IRQ_EDGE_FALL){
            char data[5] = "2GED"; // Garra Esquerda Desce
            xQueueSendFromISR(xQueueBTN, &data, 0);

        } if(events==GPIO_IRQ_EDGE_RISE){
            char data[5] = "2GES"; // Garra Esquerda Sobe
            xQueueSendFromISR(xQueueBTN, &data, 0);
        }
    }
}

void init_all(){
    gpio_init(BTN_PIN_ENTER);
    gpio_set_dir(BTN_PIN_ENTER, GPIO_IN);
    gpio_pull_up(BTN_PIN_ENTER);
    gpio_set_irq_enabled_with_callback(BTN_PIN_ENTER, GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    gpio_init(BTN_PIN_ESC);
    gpio_set_dir(BTN_PIN_ESC, GPIO_IN);
    gpio_pull_up(BTN_PIN_ESC);
    gpio_set_irq_enabled(BTN_PIN_ESC, GPIO_IRQ_EDGE_FALL, true);

    gpio_init(GARRA_DIREITA);
    gpio_set_dir(GARRA_DIREITA, GPIO_IN);
    gpio_pull_up(GARRA_DIREITA);
    gpio_set_irq_enabled(GARRA_DIREITA, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);

    gpio_init(GARRA_ESQUERDA);
    gpio_set_dir(GARRA_ESQUERDA, GPIO_IN);
    gpio_pull_up(GARRA_ESQUERDA);
    gpio_set_irq_enabled(GARRA_ESQUERDA, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);

    adc_gpio_init(BTN_PRESSION);

    gpio_init(LED_BLUETOOTH); 
    gpio_init(VIBRA);

    gpio_set_dir(LED_BLUETOOTH, GPIO_OUT);
    gpio_set_dir(VIBRA, GPIO_OUT);

    gpio_init(ANL_X);
    gpio_init(ANL_Y);
}

void ddp_task() {

  while (true) {
    adc_select_input(0);
    int ddp = adc_read();
    double ddp_volts = (double) (ddp*3)/4095;
    xQueueSend(xQueueBall, &ddp_volts, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void ball_send_task() {
  double ddp;
  int pressed = 0;

  while(true) {
    if(xQueueReceive(xQueueBall, &ddp, portMAX_DELAY)) {
        if (ddp < 2.7 && pressed == 0) {
          char data[5] = "2BDS"; // Tecla B desce
          xQueueSend(xQueueBTN, &data, 0);
          
          pressed = 1;
        }
        else if (ddp > 2.7 && pressed == 1) {
          char data[5] = "2BSB"; // Tecla B sobe
          xQueueSend(xQueueBTN, &data, 0);
          
          pressed = 0;
        }
        if(ddp<2.5){
          gpio_put(VIBRA, 1);
        } else{
          gpio_put(VIBRA, 0);
        }
      }
  }
}

void x_task() {
    int values_x[5] = {0,0,0,0,0};
    int cont_x = 0;

    while (true) {
        adc_select_input(1);
        int result_x = adc_read();
        result_x = (result_x/32) - 72;

        values_x[cont_x%5] = result_x;
        cont_x++;
            
        int soma_x = 0;
        for (int i = 0; i < 5; i++) {
            soma_x += values_x[i];
        }
        soma_x /= 5;

        if (soma_x > 25 || soma_x < -25) {
            char data[5];
            snprintf(data, sizeof(data), "0%03d", -soma_x);
            xQueueSend(xQueueMouse, &data, 0);
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}

void y_task() {
    int values_y[5] = {0,0,0,0,0};
    int cont_y = 0;

    while (true) {
        adc_select_input(2);
        int result_y = adc_read();
        result_y = (result_y/32) - 60;

        values_y[cont_y%5] = result_y;
        cont_y++;
            
        int soma_y = 0;
        for (int i = 0; i < 5; i++) {
            soma_y += values_y[i];
        }
        soma_y /= 5;

        if (soma_y > 30 || soma_y < -30) {
            char data[5];
            snprintf(data, sizeof(data), "1%03d", soma_y);
            xQueueSend(xQueueMouse, &data, 0);
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}

void hc06_task(void *p) {
    uart_init(HC06_UART_ID, HC06_BAUD_RATE);
    gpio_set_function(HC06_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(HC06_RX_PIN, GPIO_FUNC_UART);
    hc06_init("bia_ellen", "5678");

    char data[5];
    repeating_timer_t timer_0;
    int timer_setted = 0;

    while (true) {
        if(gpio_get(HC06_STATE_PIN)){
            if(timer_setted){
                cancel_repeating_timer(&timer_0);
                timer_setted = 0;
            }
            gpio_put(LED_BLUETOOTH, 1);
        } else if(!timer_setted){
            if(add_repeating_timer_us(500000, timer_0_callback, NULL, &timer_0)){
                timer_setted = 1;
            } 
        }

        if (xQueueReceive(xQueueBTN, &data, 50)) {
            uart_puts(HC06_UART_ID, data);
        } else if (xQueueReceive(xQueueMouse, &data, 50)){
            printf("%s", data);
            uart_puts(HC06_UART_ID, data);
        }   
    }
}

int main() {
    stdio_init_all();
    adc_init();
    init_all();

    printf("Start bluetooth task\n");

    xQueueBTN = xQueueCreate(64, 5*sizeof(char));
    xQueueMouse = xQueueCreate(2, 5*sizeof(char));
    xQueueBall = xQueueCreate(32, sizeof(double));

    xTaskCreate(hc06_task, "UART_Task 1", 4096, NULL, 1, NULL);
    xTaskCreate(ddp_task, "DDP", 4096, NULL, 1, NULL);
    xTaskCreate(ball_send_task, "BALL", 4096, NULL, 1, NULL);
    xTaskCreate(x_task, "X Task", 4096, NULL, 1, NULL);
    xTaskCreate(y_task, "Y Task", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
