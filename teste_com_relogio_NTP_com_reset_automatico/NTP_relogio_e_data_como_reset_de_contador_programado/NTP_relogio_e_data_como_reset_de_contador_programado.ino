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

const int pinoBotaoInc = 25; // incrementa
const int pinoBotaoReset = 26; // reseta valor
int contador = 0;
bool ultimoEstadoInc = LOW;
bool ultimoEstadoReset = LOW;
bool jaResetouHoje = false;

unsigned long tempoAnteriorSerial = 0; // Para controlar o intervalo do print

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
  
  // Inicia e sincroniza a hora via NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  preferences.begin("armazena_cont", false);
  contador = preferences.getInt("cont", 0);
  Serial.println("\nWi-Fi Conectado!");
  Serial.printf("Contador recuperado: %d\n", contador);
}

void loop() {
  struct tm timeinfo;
  bool conseguiuHora = getLocalTime(&timeinfo);

  // 1. Mostrar hora e contador no Serial a cada 1 segundo
  if (conseguiuHora && (millis() - tempoAnteriorSerial >= 1000)) {
    tempoAnteriorSerial = millis();
    char horaFormatada[9];
    strftime(horaFormatada, sizeof(horaFormatada), "%H:%M:%S", &timeinfo);
    Serial.printf("[%s] Contador: %d\n", horaFormatada, contador);
  }

  // 2. Lógica de Incremento
  if (digitalRead(pinoBotaoInc) == LOW && ultimoEstadoInc == HIGH) {
    delay(50); 
    contador++;
    preferences.putInt("cont", contador);
    Serial.println("-> Incrementado e salvo!");
  }
  ultimoEstadoInc = digitalRead(pinoBotaoInc);

  // 3. Lógica de Reset Manual
  if (digitalRead(pinoBotaoReset) == LOW && ultimoEstadoReset == HIGH) {
    delay(50);
    zerarContador();
    Serial.println("-> Reset Manual realizado!");
  }
  ultimoEstadoReset = digitalRead(pinoBotaoReset);

  // 4. Reset Automático às 06:00 AM
  if (conseguiuHora) {
    if (timeinfo.tm_hour == 6 && timeinfo.tm_min == 0 && !jaResetouHoje) {
      zerarContador();
      jaResetouHoje = true;
      Serial.println(">>> ALERTA: Memória zerada (06:00 AM)!");
    }

    if (timeinfo.tm_hour == 6 && timeinfo.tm_min == 1) {
      jaResetouHoje = false;
    }
  }
}

void zerarContador() {
  contador = 0;
  preferences.putInt("cont", 0);
}