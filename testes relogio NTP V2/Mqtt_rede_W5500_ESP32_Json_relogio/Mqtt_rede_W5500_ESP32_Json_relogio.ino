#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <NTPClient.h>
#include <EthernetUdp.h>
#include <time.h>

#define ETH_SPI_SCS 5
#define LED_PIN     2
#define BUTTON_PIN  25 // Seu botão envia HIGH quando apertado

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);
const char* mqtt_server = "192.168.1.13";

EthernetUDP ntpUDP;
NTPClient timeClient(ntpUDP, "a.st1.ntp.br", -10800, 60000); 

EthernetClient espClient;
PubSubClient client(espClient);
Preferences prefs;

unsigned long lastMsgTime = 0;
bool lastBtnState = LOW; // Inicia em LOW para lógica invertida

void enviarStatus(const char* motivo) {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)) return;

  char dataCompleta[25];
  strftime(dataCompleta, sizeof(dataCompleta), "%d/%m/%Y %H:%M:%S", &timeinfo);

  JsonDocument doc;
  doc["evento"] = motivo;
  doc["data_hora"] = dataCompleta;
  doc["led"] = digitalRead(LED_PIN) ? "ON" : "OFF";

  char buffer[256];
  serializeJson(doc, buffer);
  
  if (client.publish("esp32/mqtt/dados_json", buffer)) {
    Serial.println("Publicado com sucesso!");
  } else {
    Serial.println("Erro ao publicar!");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando MQTT...");
    // Tenta conectar com um ID único
    if (client.connect("ESP32_W5500_Final")) {
      Serial.println("Conectado ao Broker!");
      client.subscribe("esp32/mqtt/cmd");
    } else {
      Serial.print("Falha rc=");
      Serial.print(client.state());
      delay(3000); // Tenta novamente em 3s
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  // Se o botão manda HIGH, usamos INPUT_PULLDOWN (se o ESP suportar no pino 25)
  // ou apenas INPUT se houver resistor externo.
  pinMode(BUTTON_PIN, INPUT_PULLDOWN); 

  Ethernet.init(ETH_SPI_SCS);
  Ethernet.begin(mac, ip);
     
  
  
  client.setServer(mqtt_server, 1883);
  client.setBufferSize(512);
  
  timeClient.begin();
  while(!timeClient.update()){
    timeClient.forceUpdate();
    delay(500);
    Serial.print(".");
  }

  struct timeval tv = { .tv_sec = timeClient.getEpochTime() };
  settimeofday(&tv, NULL);
  setenv("TZ", "<-03>3", 1);
  tzset();
  
  prefs.begin("log_tempo", false);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();
  timeClient.update();

  // --- LÓGICA INVERTIDA DO BOTÃO (Pressionado = HIGH) ---
  bool currentBtnState = digitalRead(BUTTON_PIN);
  if (currentBtnState == HIGH && lastBtnState == LOW) { 
    digitalWrite(LED_PIN, !digitalRead(LED_PIN)); 
    enviarStatus("BOTAO_PRESSIONADO");
    delay(200); 
  }
  lastBtnState = currentBtnState;

  unsigned long now = millis();
  if (now - lastMsgTime > 5000) {
    lastMsgTime = now;
    enviarStatus("PERIODICO");
  }
}