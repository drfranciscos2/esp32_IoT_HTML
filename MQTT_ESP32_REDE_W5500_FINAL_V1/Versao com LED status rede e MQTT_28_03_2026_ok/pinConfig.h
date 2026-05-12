
// arquivo de cabeçalho .h
//nao usar os GPIOs 6 a 11 (conectados à flash integrada). e pinos de boot (0, 2, 12, 15).
//Entrada Apenas:  GPIOs 34 a 39 são apenas entrada (não têm pull-up interno).


#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H


#include <Arduino.h>
#include <Bounce2.h>

// Mapeamento dos Pinos (usando GPIOs)

// mapeamento dos pinos de entrada da placa 
const int ENTRADA_1 = 36;
const int ENTRADA_2 = 39;
const int ENTRADA_3 = 32;
const int ENTRADA_4 = 33;
const int ENTRADA_5 = 25;
const int ENTRADA_6 = 26;
const int ENTRADA_7 = 27;
const int ENTRADA_8 = 14;

// Instanciando os objetos Bounce
extern Bounce BT_1;
extern Bounce BT_2;
extern Bounce BT_3;
extern Bounce BT_4;
extern Bounce BT_5;
extern Bounce BT_6;
extern Bounce BT_7;
extern Bounce BT_8;


 




// mapeamento das saidas
const int SAIDA_LED = 2; // led embutido na placa
const int SAIDA_1 = 4;
const int SAIDA_2 = 16;
const int SAIDA_3 = 17;
//const int SAIDA_4 = 18; pino usado na w5500 SCLK
//const int SAIDA_5 = 19; pino usado na w5500 MISO
//const int SAIDA_6 = 23; pino usado na w5500 MOSI
// pino D22 RST
// pino D5 SCS

//  para os outros arquivos enxerguem os objetos do pinConfig.cpp
extern Bounce BT_1, BT_2, BT_3, BT_4, BT_5, BT_6, BT_7, BT_8;

// Declaração da função que configurará os pinos 
void configurarPinos();
void atualizarPinos();
#endif