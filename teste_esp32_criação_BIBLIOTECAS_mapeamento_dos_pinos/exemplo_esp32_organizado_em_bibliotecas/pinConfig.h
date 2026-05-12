
// arquivo de cabeçalho .h
//nao usar os GPIOs 6 a 11 (conectados à flash integrada). e pinos de boot (0, 2, 12, 15).
//Entrada Apenas:  GPIOs 34 a 39 são apenas entrada (não têm pull-up interno).


#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

#include <Arduino.h>

// Mapeamento dos Pinos (usando GPIOs)
const int SAIDA_LED = 2;       // LED embutido
const int ENTRADA_BOTAO = 14;    // Exemplo de botão
const int PIN_SENSOR = 34;   // Exemplo de entrada analógica
// mapeamento dos pinos de entrada da placa 
const int ENTRADA_1 = 36;
const int ENTRADA_2 = 39;
const int ENTRADA_3 = 32;
const int ENTRADA_4 = 33;
const int ENTRADA_5 = 25;
const int ENTRADA_6 = 26;
const int ENTRADA_7 = 27;
const int ENTRADA_8 = 14;
// mapeamento das saidas
const int SAIDA_1 = 4;
const int SAIDA_2 = 16;
const int SAIDA_3 = 17;
//const int SAIDA_4 = 18; pino usado na w5500 SCLK
//const int SAIDA_5 = 19; pino usado na w5500 MISO
//const int SAIDA_6 = 23; pino usado na w5500 MOSI
// pino D22 RST
// pino D5 SCS

// Declaração da função que configurará os pinos 
void configurarPinos();

#endif