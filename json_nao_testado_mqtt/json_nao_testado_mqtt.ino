#include <Ethernet.h>
#include <EthernetUdp.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#define ETH_SPI_SCS   5
// Configurações de rede
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);
//IPAddress gateway(192, 168, 1, 254);
//IPAddress subnet(255, 255, 255, 0);

const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_user = "";
const char* mqtt_password = "";

// Configurações do botão
const int botaoPin = 0;

// Instâncias
EthernetClient ethClient;
PubSubClient mqttClient(ethClient);

void setup() {
  Serial.begin(115200);
  Ethernet.init(ETH_SPI_SCS);
  pinMode(botaoPin, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  // Inicializa a rede Ethernet
  Ethernet.begin(mac, ip);

  Serial.println("Checando Ethernet hardware...");
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("ERRO: Ethernet hardware não detectado!");
    return;
  }
  else {
    Serial.println("Ethernet hardware detectado!");
  }

  //Check if cable is connected
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Link desligado. verifique conecção com a rede.");
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
  }
  else {
    Serial.println("Link ligado. conectado com a rede!");
    Serial.print("teste com ping: ");
    Serial.println(ip);
    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);
   
  }

  // Conecta ao servidor MQTT
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(callback);
}

void loop() {
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();

  // Lê o estado do botão
  int botaoEstado = digitalRead(botaoPin);
  if (botaoEstado == LOW) {
    // Creat um objeto JSON
    StaticJsonDocument<200> jsonDoc;
    jsonDoc["botao"] = "pressionado";

    // Converte o objeto JSON para uma string
    char jsonString[200];
    serializeJson(jsonDoc, jsonString);

    // Publica a mensagem no tópico MQTT
    mqttClient.publish("meu_topico", jsonString);
    delay(1000); // Aguarda 1 segundo para evitar publicações excessivas
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Não é necessário para este exemplo
}

void reconnect() {
  while (!mqttClient.connected()) {
    Serial.println("Conectando ao servidor MQTT...");
    if (mqttClient.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("Conectado!");
    } else {
      Serial.print("Falha, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}
