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

    char cmd_buffer[CMD_BUFFER_SIZE];
    uint32_t cmd_pos = 0;

    // Desliga os LEDs
    gpio_put(BLUE_LED_PIN, 1);
    gpio_put(GREEN_LED_PIN, 0);
    gpio_put(RED_LED_PIN, 0);

    while (true) {
        tud_task(); // Executa tarefas 

        if (tud_cdc_available()) {
            uint8_t buf[64];
            uint32_t count = tud_cdc_read(buf, sizeof(buf));

            for (uint32_t i = 0; i < count; i++) {
                char c = buf[i];

                printf("Recebido: %c\n", buf[i]); // Mostra o caractere recebido

                // Armazena no buffer até o '\n' ou '\r'
                if (c == '\n') {
                    if (cmd_pos > 0) {
                        cmd_buffer[cmd_pos] = '\0'; // Finaliza a string

                        // Remove '\r' se estiver no final
                        if (cmd_buffer[cmd_pos - 1] == '\r') {
                            cmd_buffer[cmd_pos - 1] = '\0';
                        }
                        
                        printf("Comando completo recebido: %s\n", cmd_buffer);

                        // Aqui você pode interpretar o comando, por exemplo:
                        if (strcmp(cmd_buffer, "verde") == 0) {
                            printf("Ligando o LED verde!\n");
                            gpio_put(GREEN_LED_PIN, 1);
                        } else if (strcmp(cmd_buffer, "azul") == 0) {
                            printf("Ligando o LED azul!\n");
                            gpio_put(BLUE_LED_PIN, 1);
                        } else if (strcmp(cmd_buffer, "vermelho") == 0) {
                            printf("Ligando o LED vermelho!\n");
                            gpio_put(RED_LED_PIN, 1);
                        } else if (strcmp(cmd_buffer, "branco") == 0) {
                            printf("Ligando o LED branco!\n");
                            gpio_put(RED_LED_PIN, 1);
                            gpio_put(BLUE_LED_PIN, 1);
                            gpio_put(GREEN_LED_PIN, 1);
                        }

                        // Reseta o buffer para o próximo comando
                        cmd_pos = 0;
                    }
                } else if (cmd_pos < CMD_BUFFER_SIZE - 1) {
                    cmd_buffer[cmd_pos++] = c;
                }
            }

            // Ecoa de volta para o terminal
            tud_cdc_write(buf, count);
            tud_cdc_write_flush();
        }
    }

    return 0;
}
