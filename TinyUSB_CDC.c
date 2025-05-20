#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "tusb.h"

#define RED_LED_PIN 13
#define BLUE_LED_PIN 12
#define GREEN_LED_PIN 11

#define CMD_BUFFER_SIZE 128

int main() {
    stdio_init_all();

    gpio_init(BLUE_LED_PIN);
    gpio_set_dir(BLUE_LED_PIN, GPIO_OUT);

    gpio_init(GREEN_LED_PIN);
    gpio_set_dir(GREEN_LED_PIN, GPIO_OUT);

    gpio_init(RED_LED_PIN);
    gpio_set_dir(RED_LED_PIN, GPIO_OUT);

    while (!tud_cdc_connected()) {
        sleep_ms(100);
    }

    printf("USB conectado!\n");

    while (true) {
        tud_task();

        if (tud_cdc_available()) {
            uint8_t buf[64];
            char command[CMD_BUFFER_SIZE];
            uint32_t count = tud_cdc_read(buf, sizeof(buf));

            // Copia para string e garante o terminador nulo
            if (count < CMD_BUFFER_SIZE) {
                memcpy(command, buf, count);
                command[count] = '\0';

                // Remove \r se existir (por causa de terminais que enviam \r\n)
                if (command[count - 1] == '\r') {
                    command[count - 1] = '\0';
                }

                // Compara e executa comandos
                if (strcmp(command, "verde") == 0) {
                    gpio_put(RED_LED_PIN, 0);
                    gpio_put(BLUE_LED_PIN, 0);
                    gpio_put(GREEN_LED_PIN, 1);
                } else if (strcmp(command, "vermelho") == 0) {
                    gpio_put(GREEN_LED_PIN, 0);
                    gpio_put(BLUE_LED_PIN, 0);
                    gpio_put(RED_LED_PIN, 1);
                } else if (strcmp(command, "azul") == 0) {
                    gpio_put(GREEN_LED_PIN, 0);
                    gpio_put(RED_LED_PIN, 0);
                    gpio_put(BLUE_LED_PIN, 1);
                } else if (strcmp(command, "branco") == 0) {
                    gpio_put(GREEN_LED_PIN, 1);
                    gpio_put(RED_LED_PIN, 1);
                    gpio_put(BLUE_LED_PIN, 1);
                }

                sleep_ms(1000);

                gpio_put(GREEN_LED_PIN, 0);
                gpio_put(RED_LED_PIN, 0);
                gpio_put(BLUE_LED_PIN, 0);

                printf("Comando recebido: %s\n", command);
            }

            tud_cdc_write(buf, count);
            tud_cdc_write_flush();
        }
    }

    return 0;
}
