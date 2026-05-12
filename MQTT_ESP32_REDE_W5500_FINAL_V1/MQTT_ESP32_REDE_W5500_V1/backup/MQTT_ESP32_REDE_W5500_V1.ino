

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Ethernet.h>
#include <Bounce2.h>
#include <EthernetUdp.h>
#include <NTPClient.h>
#include <TimeLib.h>
#include "pinConfig.h"
#include "HoraDataNTP.h"

#define ETH_SPI_SCS 5  // CS (Chip Select),



byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);
IPAddress gateway(192, 168, 1, 254); 
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(8, 8, 8, 8);

//const char* ssid = "Francisco";
//const char* password = "fransousa";


const char* mqtt_server = "192.168.1.13";  // servidor MQTT
const int mqtt_port = 1883;
const char* mqtt_user = "";  // usuario mqtt
const char* mqtt_pass = "";  // senha mqtt


const char* device_id = "Placa_01";  // Id da placa

// Topicos
const char* topic_telemetry = "esp32/mqtt/dados_json";  // dados Json a cada 5 segundos
const char* topic_command = "esp32/mqtt/cmd";           // comando ON ou OFF
const char* topic_status = "esp32/mqtt/status";         // status de conexão da placa


EthernetClient espClient;
PubSubClient client(espClient);

EthernetUDP ntpUDP;
NTPClient timeClient(ntpUDP, "a.st1.ntp.br", -10800);

// Timers for non-blocking delays
unsigned long lastMsgTime = 0;
const long interval = 5000;  // envia a cada 5 segundos





void checa_rede() {
  Serial.println("Checando hardware...");
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("ERRO: Defeito na W5500 ou ausente!");
    return;
  } else {
    Serial.println("Ethernet detectada!");
  }

  //Check if cable is connected
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Cabo desconectado.");
  } else {
    Serial.println("Cabo conectado!");
    Serial.print("conectado ao IP : ");
    Serial.println(ip);
  }
}
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando ao Wifi: ");
  //Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  // WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida [");
  Serial.print(topic);
  Serial.print("] ");


  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // comandos publicados
  //  se recebe "ON", liga o LED, caso receba OFF o led apaga
  if (String(topic) == topic_command) {
    if (message == "ON") {
      digitalWrite(SAIDA_LED, HIGH);
      // Feedback da publicação
      client.publish(topic_telemetry, "{\"led\": \"ON\"}");
    } else if (message == "OFF") {
      digitalWrite(SAIDA_LED, LOW);
      client.publish(topic_telemetry, "{\"led\": \"OFF\"}");
    }
  }
}

// reconecta
void reconnect() {

  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");



    if (client.connect(device_id, mqtt_user, mqtt_pass, topic_status, 1, true, "offline")) {
      Serial.println("Conectado");


      client.publish(topic_status, "online", true);


      client.subscribe(topic_command);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5s");
     static unsigned long ultimaTentativa = 0;
if (millis() - ultimaTentativa > 5000) {
    ultimaTentativa = millis();
    // tenta conectar...
}
    }
  }
}

int estadoBotao;
Gerenciador meuGerenciador;
void setup() {
  Serial.begin(115200);

  configurarPinos();  // função de configuração da pinagem do esp32
configTime(-10800, 0, "pool.ntp.org");
  meuGerenciador.iniciar();

  delay(1000);
  Serial.println("iniciando conexão Ethernet...");
  Ethernet.init(ETH_SPI_SCS);
  Ethernet.begin(mac, ip, dns, gateway, subnet); 
  timeClient.begin();
  // setup_wifi();
  checa_rede();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}


void loop() {

   
  // garante a conexão
  if (!client.connected()) {
    reconnect();
  }
  client.loop();  // mantem o mqtt ativo
  atualizarPinos();
  // teste para verifica botao entrada_05 ,pino 25
  if (BT_5.fell()) {
    digitalWrite(SAIDA_LED, HIGH);
    Serial.println("LED Aceso");
    estadoBotao = 1;
  }

  if (BT_5.rose()) {
    digitalWrite(SAIDA_LED, LOW);
    Serial.println("LED Apagado");
    estadoBotao = 0;
  }
  struct tm timeinfo;
  bool conseguiuHora = getLocalTime(&timeinfo);
  unsigned long now = millis();
  if (now - lastMsgTime > interval) {
    lastMsgTime = now;


    // cria instrução Json
    JsonDocument doc;
    if (timeClient.update()) {
    unsigned long epochTime = timeClient.getEpochTime();
    struct tm *ptm = gmtime((const time_t *)&epochTime);
    
    char dataHora[25];
    sprintf(dataHora, "%02d/%02d/%04d %02d:%02d:%02d", 
            ptm->tm_mday, ptm->tm_mon + 1, ptm->tm_year + 1900, 
            ptm->tm_hour - 3, ptm->tm_min, ptm->tm_sec); // -3 para GMT-3
    doc["Data_Hora"] = dataHora;        
    }
    doc["Placa"] = device_id;
    
    doc["tempo_millis"] = millis() / 1000;
    doc["wifi_rssi"] = random(20, 30);
    doc["botao_25"] = estadoBotao;
    // simula um valor aleatorio de um sensor
    doc["temp"] = random(20, 30);

    //  JSON para String
    char buffer[256];
    serializeJson(doc, buffer);

    // publica no  MQTT
    Serial.print("Publicação de dados Json: ");
    Serial.println(buffer);
    client.publish(topic_telemetry, buffer);
  }
}