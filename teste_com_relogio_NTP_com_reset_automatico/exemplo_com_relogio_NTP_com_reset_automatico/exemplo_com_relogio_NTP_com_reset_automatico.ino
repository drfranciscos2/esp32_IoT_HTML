#include <WiFi.h>
#include <Preferences.h>
#include "time.h"

// --- Configurações de Rede ---
const char* ssid     = "Francisco";
const char* password = "fransousa";

// --- Configurações de Hora (Brasília: -3h) ---
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -10800; 
const int   daylightOffset_sec = 0;

Preferences preferences;

const int pinoBotaoInc = 25;
const int pinoBotaoReset = 26;
int contador = 0;
bool ultimoEstadoInc = LOW;
bool ultimoEstadoReset = LOW;
bool jaResetouHoje = false;

unsigned long tempoAnteriorSerial = 0; 

void setup() {
  Serial.begin(115200);
  pinMode(pinoBotaoInc, INPUT_PULLUP);
  pinMode(pinoBotaoReset, INPUT_PULLUP);

  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500); 
    Serial.print("."); 
  }
  
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  preferences.begin("armazena_cont", false);
  contador = preferences.getInt("cont", 0);
  Serial.println("\nWi-Fi Conectado!");
}

void loop() {
  struct tm timeinfo;
  bool conseguiuHora = getLocalTime(&timeinfo);

  // 1. Mostrar DATA, HORA e CONTADOR a cada 1 segundo
  if (conseguiuHora && (millis() - tempoAnteriorSerial >= 1000)) {
    tempoAnteriorSerial = millis();
    char bufferDataHora[30];
    // %d=dia, %m=mês, %Y=ano (4 dígitos), %H=hora, %M=minuto, %S=segundo
    strftime(bufferDataHora, sizeof(bufferDataHora), "%d/%m/%Y %H:%M:%S", &timeinfo);
    
    Serial.print("[");
    Serial.print(bufferDataHora);
    Serial.print("] Contador: ");
    Serial.println(contador);
  }

  //  Lógica de Incremento e Salvamento
  if (digitalRead(pinoBotaoInc) == LOW && ultimoEstadoInc == HIGH) {
    delay(50); // Debounce
    contador++;
    preferences.putInt("cont", contador);
    Serial.printf(">> Botão Pressionado! Novo valor salvo: %d\n", contador);
  }
  ultimoEstadoInc = digitalRead(pinoBotaoInc);

  //  Lógica de Reset Manual (Botão)
  if (digitalRead(pinoBotaoReset) == LOW && ultimoEstadoReset == HIGH) {
    delay(50);
    zerarContador();
    Serial.println(">> Reset Manual via Botão!");
  }
  ultimoEstadoReset = digitalRead(pinoBotaoReset);

  //  Reset Automático às 06:00:00
  if (conseguiuHora) {
    if (timeinfo.tm_hour == 6 && timeinfo.tm_min == 0 && !jaResetouHoje) {
      zerarContador();
      jaResetouHoje = true;
      Serial.println(">>> RESET AGENDADO EXECUTADO (06:00 AM) <<<");
    }
    // destrava o reset as 06:01 da manha
    if (timeinfo.tm_hour == 6 && timeinfo.tm_min == 1) { 
      jaResetouHoje = false;
    }
  }
}

void zerarContador() {
  contador = 0;
  preferences.putInt("cont", 0);
}
