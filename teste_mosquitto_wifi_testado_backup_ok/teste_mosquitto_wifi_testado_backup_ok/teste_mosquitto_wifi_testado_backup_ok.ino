
#include <WiFi.h> //  wi-fi do ESP32 */
#include <PubSubClient.h>
#include <ArduinoJson.h>
 
// Tópico MQTT para recepção de informações do broker MQTT para ESP32
#define TOPICO_SUBSCRIBE "esp_inscreve"   
// Tópico MQTT para envio de informações do ESP32 para broker MQTT 
#define TOPICO_PUBLISH   "esp_publica"  
// id mqtt (para identificação da placa) 

#define ID_MQTT  "esp32_2"     
//nome da rede WI-FI 
const char* SSID = "Francisco"; 
//  Senha da rede WI-FI 
const char* PASSWORD = "fransousa"; 
  
// servidor broker MQTT 
const char* BROKER_MQTT = "192.168.1.13"; 
// Porta do Broker MQTT 
int BROKER_PORT = 1883;


WiFiClient espClient;
PubSubClient MQTT(espClient);


const int buttonPin = 4; // Pino do botão (ligar entre pino 4 e GND)

void init_serial(void);
void init_wifi(void);
void init_mqtt(void);
void reconnect_wifi(void); 
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void verifica_conexoes_wifi_mqtt(void);


void setup() 
{
    pinMode(buttonPin, INPUT_PULLUP);

    init_serial();
    init_wifi();
    init_mqtt();
}
void init_serial() 
{
    Serial.begin(115200);
}
 
void init_wifi(void) 
{
    delay(10);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    Serial.println("Aguarde");
    reconnect_wifi();
}

void init_mqtt(void) // conexão MQTT(endereço do  
{
    // broker e porta a ser conectada
    MQTT.setServer(BROKER_MQTT, BROKER_PORT); 
   //função chamada quando qualquer informação do tópico subescrito chega
    MQTT.setCallback(mqtt_callback);            
}
  
 //esta função é chamada toda vez que uma informação de um dos tópicos subescritos chega
          
void mqtt_callback(char* topic, byte* payload, unsigned int length) // vem da publicação do Broker no topico 
{                                                                   // envia para o esp32
    String msg;
 
    //obtem a string do payload recebido
    for(int i = 0; i < length; i++) 
    {
       char c = (char)payload[i];
       msg += c;
    }
    Serial.print("[MQTT] Mensagem recebida: ");
    Serial.println(msg);     
}
  
// reconecta-se ao broker MQTT (caso ainda não esteja conectado ou em caso de a conexão cair)
void reconnect_mqtt(void) 
{
    while (!MQTT.connected()) 
    {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) 
        {
            Serial.println("Conectado com sucesso ao broker MQTT!");
            MQTT.subscribe(TOPICO_SUBSCRIBE); 
        } 
        else
        {
            Serial.println("Falha ao reconectar no broker.");
            Serial.println("Havera nova tentatica de conexao em 2s");
            delay(2000);
        }
    }
}
  
//Função: reconecta-se ao WiFi

void reconnect_wifi() 
{
 
    if (WiFi.status() == WL_CONNECTED)
        return;
         
    WiFi.begin(SSID, PASSWORD);
     
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(100);
        Serial.print(".");
    }
   
    Serial.println();
    Serial.print("Conectado com sucesso na rede ");
    Serial.print(SSID);
    Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP());
}
// verifica o estado das conexões WiFI e ao broker MQTT. 
// Em caso de desconexão (qualquer uma das duas), a conexão é refeita.    
void verifica_conexoes_wifi_mqtt(void)

{
    // se não há conexão com o WiFI, a conexão é refeita */
    reconnect_wifi(); 
    // se não há conexão com o Broker, a conexão é refeita */
    if (!MQTT.connected()) 
        reconnect_mqtt(); 
} 
void loop()
{   
    
    // garante funcionamento das conexões WiFi e ao broker MQTT 
    verifica_conexoes_wifi_mqtt();
    // Envia frase ao broker MQTT 
   // MQTT.publish(TOPICO_PUBLISH, "ESP32 se comunicando com MQTT");
     MQTT.publish(TOPICO_PUBLISH, "esp32_2 publicando"); 

    MQTT.loop();
    delay(1000);   
}
