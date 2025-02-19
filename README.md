# Controle de LEDs RGB e Display SSD1306 com Joystick - BitDogLab

## Descrição do Projeto

Este projeto tem como objetivo explorar o uso do conversor analógico-digital (ADC) do RP2040 e o controle de LEDs RGB via PWM utilizando um joystick. Além disso, a posição do joystick é representada graficamente no display SSD1306 via protocolo I2C.

## Funcionalidades Implementadas

- **Controle dos LEDs RGB via Joystick:**
  - LED Azul: Controlado pelo eixo Y do joystick. O brilho aumenta ao mover para cima ou para baixo.
  - LED Vermelho: Controlado pelo eixo X do joystick. O brilho aumenta ao mover para a esquerda ou direita.
  - Controle feito via PWM para suavização da intensidade luminosa.
- **Exibição Gráfica no Display SSD1306:**
  - Um quadrado de 8x8 pixels se move proporcionalmente à posição do joystick.
- **Função do Botão do Joystick:**
  - Alterna o estado do LED Verde a cada acionamento.
  - Modifica o estilo da borda do display a cada pressão.
- **Função do Botão A:**
  - Ativa ou desativa o controle de LED PWM.
- **Interrupções e Debouncing:**
  - Todas as funções dos botões foram implementadas utilizando interrupções (IRQ) com tratamento de bouncing via software.

## Componentes Utilizados

- **Placa de desenvolvimento BitDogLab**
- **LED RGB (GPIOs 11, 12 e 13)**
- **Joystick (Eixo X: GPIO 26, Eixo Y: GPIO 27, Botão: GPIO 22)**
- **Botão A (GPIO 5)**
- **Display OLED SSD1306 (I2C - GPIO 14 e 15)**

## Requisitos

- Placa BitDogLab com RP2040
- Biblioteca para controle do display SSD1306
- Implementação de PWM e ADC no RP2040
- Configuração de interrupções (IRQ) para botões
- Tratamento de bouncing via software

### Link do Vídeo:



