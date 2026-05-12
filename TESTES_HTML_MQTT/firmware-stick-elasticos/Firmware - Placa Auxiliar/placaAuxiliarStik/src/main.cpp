#include <Arduino.h>

/* Entradas */
#define     PIN_BT_1    3
#define     PIN_BT_2    4
#define     PIN_BT_3    5
#define     PIN_BT_4    6
#define     PIN_BT_5    7
#define     PIN_BT_6    8

/* Saídas */
#define     PIN_OUT_1    9
#define     PIN_OUT_2    10
#define     PIN_OUT_3    11
#define     PIN_OUT_4    12
#define     PIN_OUT_5    13
#define     PIN_OUT_6    14

void setup(){
  /* Entradas */
  for(int i = 3; i <= 8; i++) pinMode(i, INPUT_PULLUP);
  /* Saídas */
  for(int i = 9; i <= 14; i++) pinMode(i, OUTPUT);
}

void loop(){
  /* Ler botões */
  for(int i = 0; i <= 5; i++){
    if(!digitalRead(3 + i)){
      for(int j = 9; j <= 14; j++) digitalWrite(j, 0); //Desliga todos os LED's
      digitalWrite(9 + i, 1); //Liga o LED correspondente
    }
  }
}
