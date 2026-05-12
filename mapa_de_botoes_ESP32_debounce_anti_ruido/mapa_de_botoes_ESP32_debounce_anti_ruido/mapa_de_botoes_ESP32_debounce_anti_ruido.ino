#include "ConfigBotoes.h"

bool estadoLed = LOW;

void setup() {
    Serial.begin(115200);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, estadoLed);

    inicializarTodosBotoes(); // Inicializa os 8 pinos manualmente
    Serial.println("Monitoramento de botões iniciado .");
}

void loop() {
    if (b25.pressionado()) {
        estadoLed = !estadoLed; // Inverte (True vira False e vice-versa)
        digitalWrite(LED_BUILTIN, estadoLed);
        Serial.print("LED alterado para: ");
        Serial.println(estadoLed ? "LIGADO" : "DESLIGADO");
    }
    // Verificação manual de cada pino
    if (b36.pressionado()) Serial.println("Botão 36 pressionado!");
    if (b39.pressionado()) Serial.println("Botão 39 pressionado!");
    if (b32.pressionado()) Serial.println("Botão 32 pressionado!");
    if (b33.pressionado()) Serial.println("Botão 33 pressionado!");
   // if (b25.pressionado()) Serial.println("Botão 25 pressionado!");
    if (b26.pressionado()) Serial.println("Botão 26 pressionado!");
    if (b27.pressionado()) Serial.println("Botão 27 pressionado!");
    if (b14.pressionado()) Serial.println("Botão 14 pressionado!");
}