
// arquivo de cabeçalho .h
//nao usar os GPIOs 6 a 11 (conectados à flash integrada). e pinos de boot (0, 2, 12, 15).
//Entrada Apenas:  GPIOs 34 a 39 são apenas entrada (não têm pull-up interno).


#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H


#include <Arduino.h>
#include <Bounce2.h>

// Mapeamento dos Pinos (usando GPIOs)

// mapeamento dos pinos de entrada da placa 
const int ENTRADA_1 = 13; //36
const int ENTRADA_2 = 12;  //39
const int ENTRADA_3 = 14; // 32
const int ENTRADA_4 = 35; // 33 // NC
const int ENTRADA_5 = 25; // 25 ok
const int ENTRADA_6 = 26; // 26 ok // entrada sensor contador
const int ENTRADA_7 = 33; // 27
const int ENTRADA_8 = 27; // 

// Instanciando os objetos Bounce
extern Bounce BT_1; // 13  // pinos do esp32 38 pinos (Fco)
extern Bounce BT_2; // 12
extern Bounce BT_3; // 14
extern Bounce BT_4; // 35 //NC
extern Bounce BT_5; // 25
extern Bounce BT_6; // 26
extern Bounce BT_7; // 33
extern Bounce BT_8; // 27


 




// mapeamento das saidas
const int SAIDA_LED = 2; // led embutido na placa
const int SAIDA_1 = 4;
const int SAIDA_2 = 16; // 16
const int SAIDA_3 = 17; // 17
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