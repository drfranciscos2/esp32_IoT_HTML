

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Ethernet.h>
//#include <botoes.h>
//#include <json_entradas.h>
// pinos de entrada
#define ETH_SPI_SCS   5   // CS (Chip Select),
#define LED_PIN 2 // led on-board da placa


byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);

//const char* ssid = "Francisco";
//const char* password = "fransousa";


const char* mqtt_server = "192.168.1.13"; // servidor MQTT
const int mqtt_port = 1883; 
const char* mqtt_user = ""; // usuario mqtt
const char* mqtt_pass = ""; // senha mqtt


const char* device_id = "Placa_01"; // Id da placa

// Topicos
const char* topic_telemetry = "esp32/mqtt/dados_json";   // dados Json a cada 5 segundos
const char* topic_command   = "esp32/mqtt/cmd";    // comando ON ou OFF
const char* topic_status    = "esp32/mqtt/status"; // status de conexão da placa 


EthernetClient espClient;
PubSubClient client(espClient);

// Timers for non-blocking delays
unsigned long lastMsgTime = 0;
const long interval = 5000; // envia a cada 5 segundos

void json_botao(){

    JsonDocument doc; 
    
    doc["botao"] = 1;

    //  JSON para String
    char buffer[256];
    serializeJson(doc, buffer);
    // publica no  MQTT
    Serial.print("Publicação do botao: ");
    Serial.println(buffer);
    client.publish(topic_telemetry, buffer);
}


void checa_rede(){
  Serial.println("Checando hardware...");
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("ERRO: Defeito na W5500 ou ausente!");
    return;
  }
  else {
    Serial.println("Ethernet detectada!");
  }

  //Check if cable is connected
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Cabo desconectado.");
  }
  else {
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
      digitalWrite(LED_PIN, HIGH);
      // Feedback da publicação
      client.publish(topic_telemetry, "{\"led\": \"ON\"}"); 
    } else if (message == "OFF") {
      digitalWrite(LED_PIN, LOW);
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
      Serial.println(" try again in 5 seconds");
      delay(5000); // Blocking delay here is acceptable as we can't operate without connection
    }
  }
}
int Entrada_05 = 25;
int estadoBotao;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(Entrada_05, INPUT);


  delay(1000);
  Serial.println("iniciando conexão Ethernet...");
  Ethernet.init(ETH_SPI_SCS); 
  Ethernet.begin(mac, ip);
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
  client.loop(); // mantem o mqtt ativo
  // verifica botao entrada 05 pino 25
if(digitalRead(Entrada_05) == 1){
    //json_botao();
    estadoBotao = 1;
   
  }else{
   estadoBotao = 0;
  }
 
  unsigned long now = millis();
  if (now - lastMsgTime > interval) {
    lastMsgTime = now;
    
    // cria instrução Json
    JsonDocument doc; 
    doc["Placa"] = device_id;
    doc["tempo_millis"] = millis() / 1000;
    doc["wifi_rssi"] = WiFi.RSSI();
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