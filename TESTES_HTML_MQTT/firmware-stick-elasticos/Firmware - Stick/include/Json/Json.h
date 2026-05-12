#ifndef JSON_H
#define JSON_H

#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "Hardware/PIN_MAP.h"
#include "MQTT/MQTT.h"


class Json {
  private:
    String id;                    // ID do pacote
    String timestamp;             // Timestamp
    String Erl_Fita;              //Enrolamento de fita  
    String Ajst_Cor;              //Ajuste de cor
    String Prod;                  //Produção
    String Manut;                 //Manutensão
    String Lpz_maquina;           //Limpeza de máquina    
    String Outros;                //Outros
    String Velocid;
    String MAC;

  public:
    // Construtor da classe, inicializa os valores com valores padrão
    Json();

    // Função para atualizar os dados do pacote
    void updateData(int packetId, String packetTimestamp, bool packetErl_Fita, bool packetAjst_Cor, bool packetProd, bool packetManut, bool packetLpz_maquina, bool packetOutros, float packetVelocid, String packetMAC);

    // Função que cria e retorna o JSON com os dados
    String createJSON();

    // Função para exibir o JSON no monitor serial
    void printJSON();

};

#endif // JSON_H