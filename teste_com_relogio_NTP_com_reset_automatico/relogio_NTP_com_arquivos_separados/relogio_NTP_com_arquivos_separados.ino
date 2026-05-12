#include <WiFi.h>
#include "HoraDataNTP.h"

const char* ssid = "Francisco";
const char* password = "fransousa";
const int pinoBotaoInc = 25;
const int pinoBotaoReset = 26;

Gerenciador meuGerenciador;
bool ultimoEstadoInc = LOW;
bool ultimoEstadoReset = LOW;

void setup() {
  Serial.begin(115200);
  pinMode(pinoBotaoInc, INPUT_PULLUP);
  pinMode(pinoBotaoReset, INPUT_PULLUP);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  
  configTime(-10800, 0, "pool.ntp.org");
  meuGerenciador.iniciar();
}

void loop() {
  struct tm timeinfo;
  bool conseguiuHora = getLocalTime(&timeinfo);

  if (conseguiuHora) {
    Serial.println(&timeinfo, "%H:%M:%S");
   // meuGerenciador.exibirStatus(&timeinfo);
    meuGerenciador.verificarResetAutomatico(&timeinfo);
  }

  // Lógica de botões (Debounce simples)
  if (digitalRead(pinoBotaoInc) == HIGH && ultimoEstadoInc == LOW) {
    delay(50);
    meuGerenciador.incrementar();
  }
  ultimoEstadoInc = digitalRead(pinoBotaoInc);

  if (digitalRead(pinoBotaoReset) == HIGH && ultimoEstadoReset == LOW) {
    delay(50);
    meuGerenciador.zerarContador();
    Serial.println("-> Reset Manual realizado!");
  }
  ultimoEstadoReset = digitalRead(pinoBotaoReset);
}