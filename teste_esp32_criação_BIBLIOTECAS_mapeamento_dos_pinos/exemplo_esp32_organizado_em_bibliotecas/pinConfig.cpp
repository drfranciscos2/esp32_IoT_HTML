// arquivo de implementacao .cpp 
// "diz" ao esp32 quais são os pinos de entrada e saida
#include "pinConfig.h"

void configurarPinos() {
  // Configuração como Saída
  pinMode(SAIDA_LED, OUTPUT);
  digitalWrite(SAIDA_LED, LOW); // Inicia desligado

  // Configuração como Entrada
  pinMode(ENTRADA_BOTAO, INPUT_PULLUP); // Usa pull-up interno
  
  // Exemplo: Pino analógico (INPUT)
  pinMode(PIN_SENSOR, INPUT);

  // configuração como entrada e saida 
  pinMode(ENTRADA_1, INPUT_PULLUP);
  pinMode(ENTRADA_2, INPUT_PULLUP);
  pinMode(ENTRADA_3, INPUT_PULLUP);
  pinMode(ENTRADA_4, INPUT_PULLUP);
  pinMode(ENTRADA_5, INPUT_PULLUP);
  pinMode(ENTRADA_6, INPUT_PULLUP);
  pinMode(ENTRADA_7, INPUT_PULLUP);
  pinMode(ENTRADA_8, INPUT_PULLUP);

  pinMode(SAIDA_1, OUTPUT);
  pinMode(SAIDA_2, OUTPUT);
  pinMode(SAIDA_3, OUTPUT);
  //pinMode(SAIDA_4, OUTPUT);
 // pinMode(SAIDA_5, OUTPUT);
 // pinMode(SAIDA_6, OUTPUT);
}