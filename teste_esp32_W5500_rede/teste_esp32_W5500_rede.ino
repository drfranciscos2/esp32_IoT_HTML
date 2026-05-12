#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>
#include <PubSubClient.h>
#define ETH_SPI_SCS   5   // CS (Chip Select), Green

// Define MAC and IP
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  delay(1000);
  Serial.println("Iniciando a conecção com Ethernet ...");

  //Set the CS pin, required for ESP32 as the arduino default is different
  Ethernet.init(ETH_SPI_SCS); 

  //Start the Ethernet connection
  Ethernet.begin(mac, ip);

    //Hardware check
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

}

void loop() {
  // put your main code here, to run repeatedly:

}