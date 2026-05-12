/*

 1. Atualizado em 27/03/2026 por Francisco das Chagas Souza filho
 2. Atualizado os botões
 3. Atualizado o LED status de conexão no led pino D2 (SAIDA_LED)
 4. Atualizado FeedBack de acionamento dos sensores no LED pino D4 (SAIDA_1)
 5. Correção do zeramento de Turnos ao ligar a placa ( verificar)
 6. Correção e edição das informações de debug na leitura serial

*/
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
#include "Sensores.h"
#include "StatusLED.h" 


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

//bool pendenciaEnvio = false; // Indica que algo mudou enquanto estava offline , adcionado
//const char* eventoPendência = ""; // adcionado 

const char* device_id = "JacGuard_09";  // Id da placa

// Topicos
const char* topic_telemetry = "esp32/mqtt/dados_json";  // dados Json a cada 5 segundos
const char* topic_command = "esp32/mqtt/cmd";           // comando ON ou OFF
const char* topic_status = "esp32/mqtt/status";         // status de conexão da placa


EthernetClient espClient;
PubSubClient client(espClient);

EthernetUDP ntpUDP;
NTPClient timeClient(ntpUDP, "a.st1.ntp.br", 0);

// Timers for non-blocking delays
unsigned long lastMsgTime = 0;
const long interval = 5000;  // envia a cada 1 segundo
Preferences memoria; // espaço na memoria flash do estado dos sensores




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
  //  se recebe "ON", liga o LED, caso receba OFF o led apaga ( estou utilizando o LED para status de conexao)
  if (String(topic) == topic_command) {
    if (message == "ON") {
     // digitalWrite(SAIDA_LED, HIGH);
      // Feedback da publicação
      client.publish(topic_telemetry, "{\"led\": \"ON\"}");
    } else if (message == "OFF") {
     // digitalWrite(SAIDA_LED, LOW);
      client.publish(topic_telemetry, "{\"led\": \"OFF\"}");
    }
  }
}

// reconecta
void reconnect() {
  static unsigned long ultimaTentativa = 0;
  if (!client.connected()) {
    unsigned long agora = millis();
    if (agora - ultimaTentativa > 5000) {
      ultimaTentativa = agora;
      Serial.print("Tentando conexão MQTT...");
      if (client.connect(device_id, mqtt_user, mqtt_pass, topic_status, 1, true, "offline")) {
        Serial.println("Conectado");
        client.publish(topic_status, "online", true);
        client.subscribe(topic_command);
      } else {
        Serial.print("falhou, rc=");
        Serial.print(client.state());
        Serial.println(" tentando novamente em 5s");
      }
    }
  }
}

//int estadoBotao;
Gerenciador meuGerenciador;

String macToString(byte mac[]) { // para mostrar o endereço MAC no JSON
  char buf[20];
  // Formata os bytes em hexadecimal com dois dígitos e separação por dois pontos
  sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(buf);
}

void publicarDados(const char* evento) {
  
 
  struct tm timeinfo;
  
  // Tenta ler o relógio interno (RTC) do ESP32
  if(!getLocalTime(&timeinfo)){
    Serial.println("Erro: RTC não sincronizado. Tentando forçar NTP...");
    timeClient.update();
    time_t t = timeClient.getEpochTime();
    struct timeval tv = { .tv_sec = t };
    settimeofday(&tv, NULL);
    if(!getLocalTime(&timeinfo)) return; // Aborta se ainda estiver sem hora
  }

  char dataHora[25];
  // strftime formata data/hora com segurança e sem erros de cálculo manual
  strftime(dataHora, sizeof(dataHora), "%d/%m/%Y %H:%M:%S", &timeinfo);
  

  JsonDocument doc;
  doc["Evento"] = evento;
  doc["Data_Hora"] = dataHora;
  doc["ID_Máquina"] = device_id;
  doc["MAC"] = macToString(mac);
 // doc["Status"] = sistemaOnline ? "Online" : "Offline";
  doc["S_Urdimento"] = estadoUrdimento;
  doc["S_trama"] = estadoTrama;
  doc["S_Enrolamento_Fita"] = estadoEnrolamento;
  doc["BT_Stop"] = estadoStop;
  doc["BT_Start"] = estadoStart;
  doc["BT_Manutenção"] = estadoManutencao;
 // doc["temp"] = random(20, 30);
  doc["Cont_A"] = cont_producao_A;
  doc["Cont_B"] = cont_producao_B;
  doc["Cont_C"] = cont_producao_C;
  char buffer[1024];
  serializeJson(doc, buffer);

 if (client.connected()) {
  bool result = client.publish(topic_telemetry, buffer);                                           // bool result = client.publish(topic_telemetry, buffer);
  if(!result) Serial.println("Erro ao publicar: JSON muito grande!");
  // FORMATAÇÃO PARA O MONITOR SERIAL (PRETTY PRINT) 
  Serial.println("--- Dados Publicados ---");
  serializeJsonPretty(doc, Serial); // Imprime direto na Serial com quebras de linha e recuo
  Serial.println("\n------------------------");                                                // if(!result) Serial.println("Erro ao publicar: JSON muito grande!");
                                                                                                //  Serial.print("Publicado: ");                                                                                                // Serial.println(buffer);
  }
}
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF); 
  // --- FORÇAR RESET DO W5500 ---
  #define W5500_RST_PIN 22  // Seu pino D22
  pinMode(W5500_RST_PIN, OUTPUT);

  digitalWrite(W5500_RST_PIN, LOW); delay(200);   // Reseta o W5500                          
  digitalWrite(W5500_RST_PIN, HIGH); delay(1000);  // Libera o CI
                          
  // -----------------------------
  Ethernet.init(ETH_SPI_SCS);

Ethernet.begin(mac, ip, dns, gateway, subnet);

  Serial.print("--------------------------> Programa Coletor de Dados MQTT para Stik Setor : Jacguard  <-------------------------------");
  Serial.print("\n------------------------> Desenvolvido por : Francisco das Chagas Souza Filho        <-------------------------------");
  Serial.print("\n------------------------>              Na Data : 29/03/2026                          <-------------------------------");
  Serial.print("\nAguardando link de rede...");
  while (Ethernet.linkStatus() == LinkOFF) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nLink detectado! Aguardando 3 segundos para estabilizacao...");
  delay(3000); // tempo para o  roteador reconhecer o ESP32 antes do MQTT

  if (Ethernet.linkStatus() == LinkON) {
    Serial.println("\n[OK] Cabo detectado! Aguardando estabilizacao da rede...");
    delay(2000); 
  }
                                                                     // Ethernet.begin(mac, ip, dns, gateway, subnet);
  
  
 
  configurarPinos(); // Inicializa hardware
  
  inicializarSensores(); // Carrega contagens da Flash

  client.setServer(mqtt_server, mqtt_port);

  //client.setCallback(callback);
  client.setBufferSize(1024); //  Aumentar buffer para o JSON caber

  Serial.println("Tentando primeira conexao MQTT...");
  reconnect(); // Chama a função de reconexão já no setup
  
  checa_rede();
  Serial.println("Sistema Iniciado!");
  
  timeClient.begin();

  int tentativasNTP = 0;                                                       // Força sincronia inicial
  while(!timeClient.update() && tentativasNTP < 10) {                           //while(!timeClient.update()){
    Serial.println("Tentando sincronizar NTP...");                             // timeClient.forceUpdate();
    timeClient.forceUpdate();                                                  // delay(500);
    delay(1000);                                                                //  Serial.print(".");
    tentativasNTP++;                                                               // }
  }                                                                                                                                                   

  // 1. Injeta o tempo UTC (puro da rede) no chip
  time_t t = timeClient.getEpochTime();
  struct timeval tv = { .tv_sec = t, .tv_usec = 0 };
  settimeofday(&tv, NULL);

  // 2. Define que o fuso local do sistema é -3 horas (Brasília)
  // Agora o getLocalTime() pegará o UTC e subtrairá 3 apenas uma vez.
  setenv("TZ", "<-03>3", 1);
  tzset();

  delay(1000);
  checa_rede();
}

void loop() {

  atualizarStatusREDE(client); // Controla o SAIDA_LED (Pino 2)// estado em que o LED vai ficar ( piscando rapido erro de conexao MQTT; pisca lento: erro cabo de rede)
  gerenciarTempoPiscaD4(); //apaga o LED D4 após o flash

 //  SE DESCONECTADO Força nível 0 em tudo e limpa o LED D4
 // if (Ethernet.linkStatus() == LinkOFF || !client.connected()) {
  //    estadoBotao = 0;
   //   estadoUrdimento = 0;
  //    estadoTrama = 0;
  //    estadoEnrolamento = 0;
  //    estadoStop = 0;
  //    estadoStart = 0;
   //   estadoManutencao = 0;
  //    digitalWrite(SAIDA_1, LOW); // Garante que o LED de feedback apague
 // }

   // Monitora as 6 entradas individualmente para o LED D4 (SAIDA_1)
  monitorarSensorParaPisca(0, estadoUrdimento, client); 
  monitorarSensorParaPisca(1, estadoTrama, client);
  monitorarSensorParaPisca(2, estadoEnrolamento, client);
  monitorarSensorParaPisca(3, estadoStop, client);
  monitorarSensorParaPisca(4, estadoStart, client);
  monitorarSensorParaPisca(5, estadoManutencao, client);
  monitorarSensorParaPisca(6, estadoBotao, client);
  
  

  if (!client.connected()) {
    reconnect();
  }else{
  client.loop();
  }  
  verificarResetDiario();
  processarSensores(client.connected()); // Gerencia Sensores , contador, se estiver desconectado da rede sensores vai para nivel 0
  
  // if (!client.connected()) {
    // Exemplo: se o botão Start foi pressionado enquanto offline
   // if (estadoStart == HIGH || estadoStop == HIGH) { 
    //  pendenciaEnvio = true;
   //   eventoPendência = "Evento_Offline"; 
  //  }
 // }

  // Se a conexão voltou e existe uma pendência, envia imediatamente
 // if (client.connected() && pendenciaEnvio) {
  //  Serial.println("Reconectado! Enviando dados capturados em modo offline...");
   // publicarDados(eventoPendência);
  //  pendenciaEnvio = false; // Reseta a flag após o envio
 // }
  
  //  ENVIO PERIÓDICO (CADA 5 SEGUNDOS) ---
  unsigned long now = millis();
  if (now - lastMsgTime > interval) {
    lastMsgTime = now;
    if (client.connected()) { // SÓ PUBLICA SE ESTIVER CONECTADO
        publicarDados("PERIODICO");
    }
  }
}
  