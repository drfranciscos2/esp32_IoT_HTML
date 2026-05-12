// arquivo de implementacao .cpp 
// "diz" ao esp32 quais são os pinos de entrada e saida
#include "pinConfig.h"
#include <Bounce2.h>

// Instanciando os objetos
Bounce BT_1 = Bounce();
Bounce BT_2 = Bounce();
Bounce BT_3 = Bounce();
Bounce BT_4 = Bounce();
Bounce BT_5 = Bounce();
Bounce BT_6 = Bounce();
Bounce BT_7 = Bounce();
Bounce BT_8 = Bounce();

void configurarPinos() {
  
 // Inicia com as saidas desligadas

  digitalWrite(SAIDA_LED, LOW); 
  digitalWrite(SAIDA_1, LOW);
  digitalWrite(SAIDA_2, LOW);
  digitalWrite(SAIDA_3, LOW);

  // configuração como entrada
 // pinMode(ENTRADA_1, INPUT_PULLUP);
 // pinMode(ENTRADA_2, INPUT_PULLUP);
 // pinMode(ENTRADA_3, INPUT_PULLUP);
 // pinMode(ENTRADA_4, INPUT_PULLUP);
 // pinMode(ENTRADA_5, INPUT_PULLUP);
 // pinMode(ENTRADA_6, INPUT_PULLUP);
 // pinMode(ENTRADA_7, INPUT_PULLUP);
 // pinMode(ENTRADA_8, INPUT_PULLUP);
  BT_1.attach(ENTRADA_1, INPUT_PULLUP);
  BT_1.interval(25);

  BT_2.attach(ENTRADA_2, INPUT_PULLUP);
  BT_2.interval(25);

  BT_3.attach(ENTRADA_3, INPUT_PULLUP);
  BT_3.interval(25);

  BT_4.attach(ENTRADA_4, INPUT_PULLUP);
  BT_4.interval(25);

  BT_5.attach(ENTRADA_5, INPUT_PULLUP);
  BT_5.interval(25);

  BT_6.attach(ENTRADA_6, INPUT_PULLUP);
  BT_6.interval(25);

  BT_7.attach(ENTRADA_7, INPUT_PULLUP);
  BT_7.interval(25);

  BT_8.attach(ENTRADA_8, INPUT_PULLUP);
  BT_8.interval(25);
  


  pinMode(SAIDA_1, OUTPUT);
  pinMode(SAIDA_2, OUTPUT);
  pinMode(SAIDA_3, OUTPUT);
  pinMode(SAIDA_LED, OUTPUT);
  //pinMode(SAIDA_4, OUTPUT);
 // pinMode(SAIDA_5, OUTPUT);
 // pinMode(SAIDA_6, OUTPUT);
}
void atualizarPinos() {
  BT_1.update();
  BT_2.update();
  BT_3.update();
  BT_4.update();
  BT_5.update();
  BT_6.update();
  BT_7.update();
  BT_8.update();
}