#include <Json/Json.h>
#include <ArduinoJson.h>
#include "Hardware/PIN_MAP.h"  
#include "MQTT/MQTT.h"

// Construtor da classe, inicializa os valores com valores padrão
Json::Json()
{
 id          = "";                    
 timestamp   = "";             
 Erl_Fita    = "";   
 Ajst_Cor    = "";
 Prod        = "";
 Manut       = "";
 Lpz_maquina = "";
 Outros      = "";
 Velocid     = "";
 MAC         = "";
}

// Função para atualizar os dados do pacote
void Json::updateData(int packetId, String packetTimestamp, bool packetErl_Fita, bool packetAjst_Cor, bool packetProd, bool packetManut, bool packetLpz_maquina, bool packetOutros, float packetVelocid, String packetMAC) {
  id          = String(packetId);  
  timestamp   = packetTimestamp;
  Erl_Fita    = String(packetErl_Fita);   
  Ajst_Cor    = String(packetAjst_Cor);
  Prod        = String(packetProd);
  Manut       = String(packetManut);
  Lpz_maquina = String(packetLpz_maquina);
  Outros      = String(packetOutros);
  Velocid     = String(packetVelocid);
  MAC         = String(packetMAC);
}

// Função que cria e retorna o JSON com os dados
String Json::createJSON() {
  // Cria o objeto JSON usando JsonDocument
  JsonDocument doc;

  // Preenche o JSON com os dados
  doc["id"]           = id;
  doc["timestamp"]    = timestamp;
  doc["Erl_Fita"]     = Erl_Fita;   
  doc["Ajst_Cor"]     = Ajst_Cor;
  doc["Prod"]         = Prod;
  doc["Manut"]        = Manut;
  doc["Lpz_maquina"]  = Lpz_maquina;
  doc["Outros"]       = Outros;
  doc["Velocid"]      = Velocid;
  doc["MAC"]          = MAC;

  // Serializa o JSON em uma String
  String output;
  serializeJson(doc, output);

  return output;  // Retorna o JSON como String
}

// Função para exibir o JSON no monitor serial
void Json::printJSON() {
  String json = createJSON();
  Serial.println(json);
}