#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "inc/ssd1306.h"

#define VRy_PIN 26
#define VRx_PIN 27
#define BOTAO_JOYSTICK 22
#define BOTAO_A 5
#define LED_PIN_G 11
#define LED_PIN_B 12
#define LED_PIN_R 13
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C 
#define PWM_WRAP 4095
#define valor_centro 2048
#define DEBOUNCE_DELAY 200  // Delay de debounce em milissegundos

bool led_g_estado = false;
bool pwm_enabled = true;
uint32_t ultimo_botao_joystick = 0;
uint32_t ultimo_botao = 0;

ssd1306_t ssd;
int estilo_borda = 0;  // Variável para controlar o estilo da borda

// Função para inicializar PWM
uint pwm_init_gpio(uint gpio) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_wrap(slice_num, PWM_WRAP);
    pwm_set_enabled(slice_num, true);
    return slice_num;
}

// Função para calcular a intensidade do LED
uint16_t calcular_intensidade_led(uint16_t value) {
    if (value > valor_centro) {
        return (value - valor_centro) * 2;
    } else if (value < valor_centro) {
        return (valor_centro - value) * 2;
    } else {
        return 0;
    }
}

// Função para desenhar a borda no display com base no estilo
void desenhar_borda(int style) {

    switch (style) {
        case 0: 
            ssd1306_rect(&ssd, 3, 3, 122, 58, true, false);
            break;
        case 1:
            ssd1306_rect(&ssd, 4, 4, 120, 56, true, false);
            ssd1306_rect(&ssd, 1, 1, 126, 62, true, false);
            ssd1306_rect(&ssd, 2, 2, 124, 60, true, false);
            ssd1306_rect(&ssd, 3, 3, 122, 58, true, false);
            break;
        case 2:
            ssd1306_fill(&ssd, false);  // Limpa o display
            break;
        default:
            break;
    }

    ssd1306_send_data(&ssd);  // Envia os dados de borda para o display
}

void gpio_callback(uint gpio, uint32_t events) {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());

    // Verifica debounce para o botão do joystick
    if (gpio == BOTAO_JOYSTICK && (current_time - ultimo_botao_joystick >= DEBOUNCE_DELAY)) {
        ultimo_botao_joystick = current_time;
        led_g_estado = !led_g_estado;
        gpio_put(LED_PIN_G, led_g_estado);

        // Alterna o estilo da borda
        estilo_borda = (estilo_borda + 1) % 3;  // Alterna entre 0, 1, 2
        desenhar_borda(estilo_borda);  // Atualiza a borda no display
    }
    
    // Verifica debounce para o botão A
    if (gpio == BOTAO_A && (current_time - ultimo_botao >= DEBOUNCE_DELAY)) {
        ultimo_botao = current_time;
        pwm_enabled = !pwm_enabled;
        if (!pwm_enabled) {
            pwm_set_gpio_level(LED_PIN_R, 0);
            pwm_set_gpio_level(LED_PIN_B, 0);
        }
    }
}
float posicao_x_anterior = -1;
float posicao_y_anterior = -1;

int main() {
    stdio_init_all();
    adc_init();

    // Configurar ADC para os pinos do joystick
    adc_gpio_init(VRy_PIN);
    adc_gpio_init(VRx_PIN);

    // Inicializar PWM para LEDs
    uint pwm_slice_R = pwm_init_gpio(LED_PIN_R);
    uint pwm_slice_B = pwm_init_gpio(LED_PIN_B);

    // Configurar pinos de botão
    gpio_init(BOTAO_JOYSTICK);
    gpio_set_dir(BOTAO_JOYSTICK, GPIO_IN);
    gpio_pull_up(BOTAO_JOYSTICK);
    gpio_set_irq_enabled_with_callback(BOTAO_JOYSTICK, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    
    gpio_init(BOTAO_A);
    gpio_set_dir(BOTAO_A, GPIO_IN);
    gpio_pull_up(BOTAO_A);
    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    // Configurar LED Verde
    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    // Inicialização do I2C e do display SSD1306
    i2c_init(I2C_PORT, 400 * 1000);  // Inicializa o I2C com taxa de 400 kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);  // Configura o pino SDA para I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);  // Configura o pino SCL para I2C
    gpio_pull_up(I2C_SDA);  // Ativa o pull-up no pino SDA
    gpio_pull_up(I2C_SCL);  // Ativa o pull-up no pino SCL

    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);  // Inicializa o display SSD1306
    ssd1306_config(&ssd);  // Configura o display
    ssd1306_send_data(&ssd);  // Envia os dados de configuração para o display
    ssd1306_fill(&ssd, false);  // Limpa o display
    ssd1306_send_data(&ssd);  // Envia os dados de limpeza para o display

    uint32_t ultimo_tempo = 0;

    while (true) {
        // Ler valores do ADC
        adc_select_input(0);  
        uint16_t vry_value = adc_read();  

        adc_select_input(1);
        uint16_t vrx_value = adc_read();

        if (pwm_enabled) {
            // Ajustar brilho dos LEDs proporcionalmente ao joystick
            uint16_t led_intensidade_B = calcular_intensidade_led(vry_value);
            uint16_t led_intensidade_R = calcular_intensidade_led(vrx_value);

            pwm_set_gpio_level(LED_PIN_B, led_intensidade_B);
            pwm_set_gpio_level(LED_PIN_R, led_intensidade_R);
        }

        float posicao_y = (4095 - (float)vry_value) / 4095 * 56;
        float posicao_x = (float)vrx_value / 4095 * 120;

        // Limpar a posição anterior, se houver
        if (posicao_y_anterior != -1 && posicao_x_anterior != -1) {
            ssd1306_rect(&ssd, posicao_y_anterior, posicao_x_anterior, 8, 8, false, true);  // Apaga o ponto anterior
        }
        desenhar_borda(estilo_borda);

        // Atualiza as variáveis da posição anterior
        posicao_x_anterior = posicao_x;
        posicao_y_anterior = posicao_y;

        ssd1306_rect(&ssd, posicao_y, posicao_x, 8, 8, true, false);
        ssd1306_send_data(&ssd);

        float duty_cycle_R = (calcular_intensidade_led(vry_value) / 4095.0) * 100;
        float duty_cycle_B = (calcular_intensidade_led(vrx_value) / 4095.0) * 100;

        uint32_t current_time = to_ms_since_boot(get_absolute_time());
        if (current_time - ultimo_tempo >= 1000) {
            printf("VRx: %u | Duty Cycle R: %.0f%%\n", vry_value, duty_cycle_R);
            printf("VRy: %u | Duty Cycle B: %.0f%%\n", vrx_value, duty_cycle_B);
            ultimo_tempo = current_time;
        }

        sleep_ms(100);
    }

    return 0;
}