#ifndef STATUS_LED_H
#define STATUS_LED_H

#include <Arduino.h>
#include <Ethernet.h>
#include <PubSubClient.h>

#define LED_PIN 2

// Tempos de pisca (em milissegundos)
#define PISCA_LENTO 1000  // Erro de Hardware/Cabo
#define PISCA_RAPIDO 150  // Erro de Conexão MQTT (Broker)

static unsigned long lastMillisLED = 0;
static bool estadoLED = LOW;

void configurarLED() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void atualizarStatusLED(PubSubClient &mqttClient) {
  unsigned long currentMillis = millis();
  int intervalo = 0;

  // 1. TUDO OK: Cabo conectado e MQTT conectado
  if (Ethernet.linkStatus() == LinkON && mqttClient.connected()) {
    digitalWrite(LED_PIN, HIGH);
    return; // Sai da função, LED fica aceso
  }

  // 2. ERRO DE CABO: Link OFF ou Hardware ausente
  if (Ethernet.linkStatus() == LinkOFF || Ethernet.hardwareStatus() == EthernetNoHardware) {
    intervalo = PISCA_LENTO;
  } 
  // 3. ERRO DE BROKER: Tem cabo, mas o MQTT caiu
  else if (!mqttClient.connected()) {
    intervalo = PISCA_RAPIDO;
  }

  // Lógica de piscar não bloqueante
  if (currentMillis - lastMillisLED >= intervalo) {
    lastMillisLED = currentMillis;
    estadoLED = !estadoLED;
    digitalWrite(LED_PIN, estadoLED);
  }
}

#endif