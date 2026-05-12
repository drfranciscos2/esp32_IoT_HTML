// arquivo principal

#include "pinConfig.h"

void setup() {
  Serial.begin(115200);
  
  // Chama a função de configuração do outro arquivo
  configurarPinos();
  
  Serial.println("Pinos configurados com sucesso.");
}

void loop() {
  // Exemplo de uso
  if (digitalRead(ENTRADA_BOTAO) == LOW) {
    digitalWrite(SAIDA_LED, HIGH);
  } else {
    digitalWrite(SAIDA_LED, LOW);
  }
}
