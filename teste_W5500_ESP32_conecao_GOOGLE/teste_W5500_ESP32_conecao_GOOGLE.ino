#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);


void setup() {
  Ethernet.begin(mac, ip);
  Serial.begin(9600);

  delay(1000); // Aguarde a inicialização da Ethernet
  Serial.println("Conectando ao site do Google...");

  if (client.connect("www.google.com", 80)) {
    Serial.println("Conectado ao site do Google!");
    client.println("GET / HTTP/1.1");
    client.println("Host: www.google.com");
    client.println("Connection: close");
    client.println();
  } else {
    Serial.println("Falha na conexão ao site do Google.");
  }
}

void loop() {
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  if (!client.connected()) {
    Serial.println();
    Serial.println("Conexão encerrada.");
    while (true);
  }
}