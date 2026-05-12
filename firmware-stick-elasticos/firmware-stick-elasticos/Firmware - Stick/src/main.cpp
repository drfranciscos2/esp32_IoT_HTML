#include <Arduino.h>
#include "Hardware/PIN_MAP.h"
#include <Ethernet.h>
#include "Json/Json.h"
#include "MQTT/MQTT.h"
#include <EthernetUdp.h>
#include <SPIFFS.h>
#include <Server.h>
#include <NTPClient.h>
#include "Sensor/veloc_1.h"

// MAC

//byte mac_ETH[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}; // Dispositivo 01   
//byte mac_ETH[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE}; // Dispositivo 02
//byte mac_ETH[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF}; // Dispositivo 03
//byte mac_ETH[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xF0}; // Dispositivo 04
byte mac_ETH[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xF1}; // Dispositivo 05
//byte mac_ETH[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xF2}; // Dispositivo 06
//byte mac_ETH[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xF3}; // Dispositivo 07
//byte mac_ETH[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xF4}; // Dispositivo 08
//byte mac_ETH[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xF5}; // Dispositivo 09
//byte mac_ETH[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xF6}; // Dispositivo 10
//byte mac_ETH[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xF7}; // Dispositivo 11
//byte mac_ETH[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xF8}; // Dispositivo 12
//byte mac_ETH[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xF9}; // Dispositivo 13
//byte mac_ETH[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xFA}; // Dispositivo 14
//byte mac_ETH[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xFB}; // Dispositivo 15
//byte mac_ETH[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xFC}; // Dispositivo 16
//byte mac_ETH[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xFD}; // Dispositivo 17
//byte mac_ETH[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xFE}; // Dispositivo 18
//byte mac_ETH[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xFF}; // Dispositivo 19
//byte mac_ETH[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x00}; // Dispositivo 20
//byte mac_ETH[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x01}; // Dispositivo 21

// MQTT

const char* mqttServer      = "45.160.103.154"; // Endereço do broker Novo. 
//const char* mqttServer      = "45.235.126.17"; // Endereço do broker Antigo.
const int   mqttPort        =  1885;
const char* mqttUser        = "usuario_mqtt";  //  usuário MQTT
const char* mqttPassword    = "Vic@1914";   //  senha MQTT
const char  mqttTopic[100]  = "stik/data";  //  topic

// NTP

NTPClient *timeClient = nullptr;  

unsigned long lastNTPUpdate = 0;
const unsigned long ntpUpdateInterval = 3600000; // 1 hora

// JSON

int     id          = 0;                    
String  timestamp   = "";             
bool    Erl_Fita    = false;   
bool    Ajst_Cor    = false;
bool    Prod        = false;
bool    Manut       = false;
bool    Lpz_maquina = false;
bool    Outros      = false;
float   Velocid     = 0.0;
int     lastVeloc   = 0;
int     veloc_100   = 0;
String  MAC         = "";

String lastJsonSent = "";

unsigned long lastSendTime = 0;
const unsigned long sendInterval = 60000;  // 1 minuto

// Botões

const int buttonPins[6] = {BT_01, BT_02, BT_03, BT_04, BT_05, BT_06};

bool buttonFlags[6] = {Erl_Fita, Ajst_Cor, Prod, Manut, Lpz_maquina, Outros,};

bool lastButtonState[6] = {LOW, LOW, LOW, LOW, LOW, LOW};

unsigned long lastDebounceTime[6] = {0, 0, 0, 0, 0, 0};
const unsigned long debounceDelay = 50;  // 50ms para evitar leituras instáveis

bool buttonStateChanged = false;
     
// -- Instancia de Biblioteca

EthernetServer      SERVIDOR(port_server);
MQTTClient          mqttClient(mqttServer, mqttPort, mqttUser, mqttPassword, mac_ETH);
Json                json;
EthernetUDP         udp_eth;
EthernetClient      client_eth;
UDP                 *ntp_udp;
Client              *client;
File                file;
LerVeloc            leituraVeloc(S_PULSO);

// -- Declaração de Funções --

void memory_init();
void Config_Rede();
void Send_Json();
void readButtons();
void updateJsonAndSend(int buttonIndex);
void updateJsonAndSendVeloc();
void setupButtons();
void sendPeriodicJson();
void page_web(String page);
void send_page();
void Time();

//bool valid_page();

TaskHandle_t tarefaHandleOta = NULL;  // Handle da tarefa do OTA

void tarefaOta(void *pvParameters){
    leituraVeloc.begin();
    while(1){
        /* Velocidade */
        Velocid = leituraVeloc.getVeloc(); //Ler velocidade
        veloc_100 = Velocid*100;
        if(((veloc_100 - lastVeloc) <= -10 ) || ((veloc_100 - lastVeloc) >= 10 )){ //Envia se mudança maior que +-10
            //Serial.println("-------------------> Veloc: " + String(veloc_100) + " ---> lastVeloc: " + String(lastVeloc));
            updateJsonAndSendVeloc();
            leituraVeloc.startBorda = 1;
        }
        lastVeloc = veloc_100;
        delay(5);
    }
}

// -- Função Principal --

void setup()
{   
    client = &client_eth;
    ntp_udp = &udp_eth;

    for(int a = 0; a < 6; a++)
    {
    if (mac_ETH[a] < 0x10) {
      MAC += "0"; // Adiciona um zero à esquerda para bytes menores que 0x10
    }
    MAC += String(mac_ETH[a], HEX);
    if (a < 5) {
      MAC += ":"; // Adiciona um separador ":" entre os bytes, exceto após o último byte
    }
    }
    MAC.toUpperCase();

    udp_eth.begin(123);
    Serial.begin(115200);
    memory_init();
    Config_Rede();
    setupButtons();
    Time();
    page_web("/login_page2.html");
    
    xTaskCreatePinnedToCore(tarefaOta, "TarefaOta", 2048, NULL, 2, &tarefaHandleOta, 0);
};

void loop()
{   
    while (Ethernet.linkStatus() == LinkOFF)
    {
        digitalWrite(LED_NET,LOW);

        Serial.println("Rede Desconectada!");
        Serial.println("Tentando Reconexão...");
        Config_Rede();
    }
    
    if (millis() - lastNTPUpdate > ntpUpdateInterval) {
        Time();
        lastNTPUpdate = millis();
    }

    mqttClient.loop();
    readButtons();
    sendPeriodicJson();
};

// -- Definição das Funções --

void memory_init()
{
    if (!SPIFFS.begin(true))
    {
        Serial.println("Erro ao inicializar o SPIFFS");
    }

    Serial.println("Sistema de Arquivos iniciado com sucesso!");
}

void Config_Rede()
{
    pinMode(LED_SEND,OUTPUT);
    pinMode(LED_NET,OUTPUT);
    pinMode(LED_INPUT,OUTPUT);

    digitalWrite(LED_SEND,LOW);
    digitalWrite(LED_NET,LOW);
    digitalWrite(LED_INPUT,LOW);
    
    Ethernet.init(CS);
    Serial.println("Inicializando a rede....");
    Ethernet.begin(mac_ETH);

    
    if (!Ethernet.begin(mac_ETH)) 
    {
        Serial.println("Failed to configure Ethernet using DHCP");
        if (Ethernet.hardwareStatus() == EthernetNoHardware)
        {
        Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
        } else if (Ethernet.linkStatus() == LinkOFF) 
            {
                Serial.println("Ethernet cable is not connected.");
            }
    }else
    {
        
        Serial.println("Sucess to configure Ethernet using DHCP");
        if (Ethernet.hardwareStatus() == 3)
        {
            Serial.println("Hardware W5500 encontrado" );
        }

        if(Ethernet.linkStatus() == LinkON)
        {
            Serial.println("Conectado a Rede!");
            digitalWrite(LED_NET,HIGH);
            SERVIDOR.begin();
            Serial.print("Servidor Disponivel em: ");
            Serial.println(Ethernet.localIP());
        }

        mqttClient.begin();
        
    }
}

void Send_Json()
{
  
        // Verifica se está conectado à rede WiFi

    if (Ethernet.linkStatus() == LinkON) 
    {  

        // Gera o JSON a ser enviado
        String aux_json = json.createJSON();
        
        Serial.println("Enviando dados para o Broker...");
        json.printJSON();

       // Envia os dados JSON para topico MQTT
        mqttClient.publish(mqttTopic,aux_json.c_str());
        Serial.println("Dados enviados com sucesso para o MQTT.");
        digitalWrite(LED_SEND,HIGH);
        delay(250);
        digitalWrite(LED_SEND,LOW);
        delay(250);
        digitalWrite(LED_SEND,HIGH);
        delay(250);
        digitalWrite(LED_SEND,LOW);

    } else
      {
      Serial.println("Erro JSON: Não conectado à rede.");
      }
}

void readButtons()
{
    for (int i = 0; i < 6; i++)
    {
        int reading = digitalRead(buttonPins[i]);  // Lê o estado atual do botão
        
        
        // Verifica se houve mudança no estado do botão
        if (reading != lastButtonState[i]) 
        {
            // Se o tempo de debounce passou e o estado do botão estabilizou
            if ((millis() - lastDebounceTime[i]) > debounceDelay) 
            {
                // Se o botão foi solto (transição de LOW para HIGH)
                //Serial.println(String(i) + " ativado!");
                digitalWrite(LED_INPUT,HIGH);
                delay(250);
                digitalWrite(LED_INPUT,LOW);
                
                // Exibe o novo estado no Serial Monitor
                Serial.print("Botão ");
                Serial.print(i+1);
                Serial.println(" ativado!");

                if (lastButtonState[i] == LOW && reading == HIGH)
                {
                    // Desativa todas as flags antes de ativar o novo botão
                    for (int j = 0; j < 6; j++)
                    {
                        buttonFlags[j] = false;
                    }
                    Serial.println("Flags Resetadas!");
                }

                // Ativa apenas o botão pressionado
                buttonFlags[i] = true;
                
                // Atualiza o JSON e envia os dados
                
                updateJsonAndSend(i);
            
                buttonStateChanged = true;

                
            }

            lastDebounceTime[i] = millis();  // Atualiza o tempo de debounce
        }

        // Atualiza o último estado do botão
        lastButtonState[i] = reading;
    }
}

void setupButtons()
{
    for (int i = 0; i < 6; i++)
    {
        pinMode(buttonPins[i], INPUT);
    }
}

void updateJsonAndSend(int buttonIndex)
 {
    // Simulação dos dados (substitua pelos valores reais conforme necessário)
    id = id+1; 
    Time();
    
    // Flags específicas para cada botão
    Erl_Fita = (buttonIndex == 0);
    Ajst_Cor = (buttonIndex == 1);
    Prod = (buttonIndex == 2);
    Manut = (buttonIndex == 3);
    Lpz_maquina = (buttonIndex == 4);
    Outros = (buttonIndex == 5);

    // Atualiza os dados no JSON
    json.updateData(id, timestamp, Erl_Fita, Ajst_Cor, Prod, Manut, Lpz_maquina, Outros, Velocid, MAC);

    lastJsonSent = json.createJSON();  // Salva o último JSON enviado

    //json.printJSON();

    // Envia o JSON via MQTT
    Send_Json();
}

void updateJsonAndSendVeloc()
 {
    // Simulação dos dados (substitua pelos valores reais conforme necessário)
    id = id+1; 
    Time();

    // Atualiza os dados no JSON
    json.updateData(id, timestamp, Erl_Fita, Ajst_Cor, Prod, Manut, Lpz_maquina, Outros, Velocid, MAC);

    lastJsonSent = json.createJSON();  // Salva o último JSON enviado

    //json.printJSON();

    // Envia o JSON via MQTT
    Send_Json();
}

void sendPeriodicJson()
 {
    if (millis() - lastSendTime >= sendInterval) {  // Verifica se já passou 1 minuto
        lastSendTime = millis();                    // Atualiza o tempo do último envio

        // Se houve uma mudança recente no estado dos botões, já enviamos o JSON atualizado
        if (buttonStateChanged) {
            buttonStateChanged = false;  // Resetar a flag após envio
        } else {
            // Se não houve mudanças, reenvia o último JSON armazenado
            if (lastJsonSent != "") {  // Garante que há um JSON válido salvo
                Send_Json();
                //Serial.println("JSON repetido enviado (sem alterações)!");
                //json.printJSON();
            }
        }
    }
}

void page_web(String page)
{
    if (SPIFFS.exists(page))
    {
        Serial.println("Arquivo Encontrado: " + page);
        file = SPIFFS.open(page);
    }else{
        Serial.println("Arquivo" + page + "nao encontrado.");
    }    
}

void send_page()
{
    /*EthernetClient client = SERVIDOR.available();

    if (client)
    {
        bool currentLineIsBlank = true;
        if(client.connected())
        {
            while (client.available())
            {
                char c = client.read();
                if (c == '\n' && currentLineIsBlank)
                {
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: close");
                    client.println();
                       
                    if (file) 
                    {
                        while (file.available())
                        {
                            client.write(file.read());
                        }      
                            file.close();
                    }
                    
                    break; // Finaliza a resposta
                }
            
                if (c == '\n')
                {
                    currentLineIsBlank = true;
                }else if (c != '\r')
                {
                    currentLineIsBlank = false;
                }
            }        
        }

        client.stop();    
    }*/
        
        
}

void Time() {
    // Verifica se há conexão de rede
    if (Ethernet.linkStatus() != LinkON) {
        Serial.println("Sem conexão de rede para NTP");
        timestamp = "1970-01-01 00:00:00";
        return;
    }

    // Lista atualizada de servidores NTP confiáveis
    const char* ntpServers[] = {
    "216.239.35.0",     // time.google.com (Google)
    "208.79.49.7",      // time.windows.com (Microsoft)
    "200.160.7.186",    // s1.ntp.br (NIC.br)
    "200.20.186.76",    // br.pool.ntp.org (Pool BR 1)
    "200.160.0.8",      // br.pool.ntp.org (Pool BR 2)
    "162.159.200.123",  // pool.ntp.org (Global 1)
    "162.159.200.1"     // pool.ntp.org (Global 2)
};
    const int ntpServerCount = sizeof(ntpServers)/sizeof(ntpServers[0]);

    if (timeClient == nullptr) {
        timeClient = new NTPClient(*ntp_udp, ntpServers[0], -3 * 3600, 60000);
        timeClient->begin();  // Remove a verificação de retorno pois begin() é void
    }

    bool synchronized = false;
    
    for (int i = 0; i < ntpServerCount; i++) {
        timeClient->setPoolServerName(ntpServers[i]);
        Serial.print("Tentando sincronizar com ");
        Serial.println(ntpServers[i]);
        
        if (timeClient->forceUpdate()) {
            Serial.println("Sincronizado com sucesso!");
            synchronized = true;
            break;
        }
        delay(1000); // Tempo adequado entre tentativas
    }

    if (!synchronized) {
        Serial.println("Falha na sincronização com todos os servidores NTP");
        timestamp = "1970-01-01 00:00:00";
        return;
    }

    // Verificação adicional do timestamp
    unsigned long epochTime = timeClient->getEpochTime();
    if (epochTime < 1600000000) { // Se anterior a 2020-09-13
        Serial.println("Timestamp inválido recebido");
        timestamp = "1970-01-01 00:00:00";
        return;
    }

    // Formatação do timestamp
    time_t epoch = (time_t)epochTime;
    struct tm *ptm = gmtime(&epoch);
    char buffer[80];
    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", ptm);
    timestamp = String(buffer);
    Serial.print("Hora atual sincronizada: ");
    Serial.println(timestamp);
}
