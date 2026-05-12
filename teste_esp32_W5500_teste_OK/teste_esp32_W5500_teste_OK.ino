#include <Ethernet.h>

#define ETH_SPI_SCS   5   // CS (Chip Select), Green

// Define MAC and IP
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("iniciando conexão Ethernet...");

  //Set the CS pin, required for ESP32 as the arduino default is different
  Ethernet.init(ETH_SPI_SCS); 

  //Start the Ethernet connection
  Ethernet.begin(mac, ip);

    //Hardware check
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

void loop() {
  // put your main code here, to run repeatedly:

}