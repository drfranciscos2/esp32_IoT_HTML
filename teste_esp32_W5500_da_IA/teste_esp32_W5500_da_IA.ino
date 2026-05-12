#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

// Configurações de Hardware (Pinos do post anterior)
const int chipSelect = 5;

// Configurações de Rede
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress server(192, 168, 1, 13); // IP do seu Broker MQTT

EthernetClient ethClient;
PubSubClient client(ethClient);

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    // Cria um ID de cliente único
    if (client.connect("ESP32Client_W5500")) {
      Serial.println("conectado");
      client.publish("esp32/status", "online");
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // Inicializa Ethernet com DHCP
  Ethernet.init(chipSelect);
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Falha ao configurar Ethernet via DHCP");
  }
  
  client.setServer(server, 1883); // Porta padrão MQTT: 1883
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Exemplo de publicação a cada 10 segundos
  static unsigned long lastMsg = 0;
  if (millis() - lastMsg > 10000) {
    lastMsg = millis();
    client.publish("esp32/dados", "Ola do W5500!");
  }
}