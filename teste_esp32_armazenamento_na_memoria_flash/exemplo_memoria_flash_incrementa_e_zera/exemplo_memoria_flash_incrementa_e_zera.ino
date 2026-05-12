#include <Preferences.h>

Preferences preferences;

const int pinoBotaoInc = 25;   // Botão para incrementar
const int pinoBotaoReset = 26; // Botão para zerar (Reset)
int contador = 0;

bool ultimoEstadoInc = LOW;
bool ultimoEstadoReset = LOW;

void setup() {
  Serial.begin(115200);
  
  // Configura os botões com resistores internos de Pull-up
  pinMode(pinoBotaoInc, INPUT_PULLUP);
  pinMode(pinoBotaoReset, INPUT_PULLUP);

// Abre a "pasta" de armazenamento chamada "armazena_cont" (falso = leitura e escrita)
  preferences.begin("armazena_cont", false);
  contador = preferences.getInt("cont", 0);

  Serial.printf("Sistema iniciado. Contador atual: %d\n", contador);
}

void loop() {
  bool estadoInc = digitalRead(pinoBotaoInc);
  bool estadoReset = digitalRead(pinoBotaoReset);

  // Lógica para INCREMENTAR
  if (ultimoEstadoInc == LOW && estadoInc == HIGH) {
    delay(50); // Debounce
    contador++;
    preferences.putInt("cont", contador);
    Serial.printf("Incrementado: %d\n", contador);
  }
  ultimoEstadoInc = estadoInc;

  // Lógica para ZERAR (RESET)
  if (ultimoEstadoReset == LOW && estadoReset == HIGH) {
    delay(50); // Debounce
    contador = 0;
    preferences.putInt("cont", contador);
    // Alternativamente, use preferences.remove("cont") para excluir a chave
    Serial.println("Contador resetado para 0!");
  }
  ultimoEstadoReset = estadoReset;
}