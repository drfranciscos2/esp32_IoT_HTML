#ifndef STATUS_LED_H
#define STATUS_LED_H

#include <Arduino.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include "pinConfig.h"

static unsigned long lastMillisStatus = 0;
static unsigned long tempoLedFeedback = 0;
static bool ledFeedbackAtivo = false;

// Array para guardar o último estado de cada um dos 8 sensores
static int ultimosEstadosSensores[8] = {0, 0, 0, 0, 0, 0, 0, 0};

//  LED 2: Status de Rede e MQTT 
void atualizarStatusREDE(PubSubClient &mqttClient) {
  unsigned long currentMillis = millis();
  if (Ethernet.linkStatus() == LinkOFF) {
    if (currentMillis - lastMillisStatus >= 1000) {
      lastMillisStatus = currentMillis;
      digitalWrite(SAIDA_LED, !digitalRead(SAIDA_LED));
    }
    return;
  }
  if (!mqttClient.connected()) {
    if (currentMillis - lastMillisStatus >= 150) {
      lastMillisStatus = currentMillis;
      digitalWrite(SAIDA_LED, !digitalRead(SAIDA_LED));
    }
    return;
  }
  digitalWrite(SAIDA_LED, HIGH);
}

//  sensores com trava de MQTT 
void monitorarSensorParaPisca(int id, int estadoSensor, PubSubClient &mqttClient) {
  // SÓ PROCESSA O PISCA SE O MQTT ESTIVER CONECTADO
  if (mqttClient.connected()) {
    // Detecta a mudança de 0 para 1 (borda de subida)
    if (estadoSensor == 1 && ultimosEstadosSensores[id] == 0) {
      digitalWrite(SAIDA_1, HIGH); // Liga o LED D4
      tempoLedFeedback = millis();
      ledFeedbackAtivo = true;
    }
  } else {
    // Se o MQTT cair, garanti que o LED D4 fique apagado
    digitalWrite(SAIDA_1, LOW);
    ledFeedbackAtivo = false;
  }
  
  // evita pisca falso se a conexão voltar
  ultimosEstadosSensores[id] = estadoSensor; 
}

//  Desliga o LED D4 após o tempo de flash (150ms) 
void gerenciarTempoPiscaD4() {
  if (ledFeedbackAtivo && (millis() - tempoLedFeedback >= 150)) {
    digitalWrite(SAIDA_1, LOW);
    ledFeedbackAtivo = false;
  }
}

#endif