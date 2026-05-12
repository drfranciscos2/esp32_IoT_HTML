/*
  Firmware V2 SEGURA - ESP32 + W5500 + MQTT
  Ajustes principais:
  - Boot não bloqueia se cabo de rede estiver desconectado
  - Callback MQTT habilitado
  - Fila simples em RAM para eventos enquanto MQTT estiver offline
  - JSON com campos antigos e campos novos padronizados para transição
  - Publicação periódica não entra na fila offline
*/

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Ethernet.h>
#include <Bounce2.h>
#include <EthernetUdp.h>
#include <NTPClient.h>
#include <TimeLib.h>
#include <Preferences.h>
#include <sys/time.h>

#include "pinConfig.h"
#include "HoraDataNTP.h"
#include "Sensores.h"
#include "StatusLED.h"

#define ETH_SPI_SCS 5
#define W5500_RST_PIN 22

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);
IPAddress gateway(192, 168, 1, 254);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(8, 8, 8, 8);

const char* mqtt_server = "192.168.1.13";
const int mqtt_port = 1883;
const char* mqtt_user = "";
const char* mqtt_pass = "";

const char* device_id = "JacGuard_09";

// Mantido por compatibilidade com o HTML atual.
const char* topic_telemetry = "esp32/mqtt/dados_json";
const char* topic_command   = "esp32/mqtt/heartbeat";//"esp32/mqtt/cmd";
const char* topic_status    = "esp32/mqtt/";//"esp32/mqtt/status";

EthernetClient espClient;
PubSubClient client(espClient);
EthernetUDP ntpUDP;
NTPClient timeClient(ntpUDP, "a.st1.ntp.br", 0);

unsigned long lastMsgTime = 0;
const unsigned long interval = 5000;

unsigned long lastNetworkCheck = 0;
const unsigned long networkCheckInterval = 10000;

// Fila simples em RAM para eventos críticos durante queda de MQTT.
// Observação: se faltar energia, essa fila é perdida. Para produção final,
// o ideal é uma fila persistente ou backend com confirmação de recebimento.
const int EVENT_QUEUE_SIZE = 30;
const int EVENT_NAME_SIZE = 48;
char eventQueue[EVENT_QUEUE_SIZE][EVENT_NAME_SIZE];
int eventHead = 0;
int eventTail = 0;
int eventCount = 0;

bool isPeriodicEvent(const char* evento) {
  return strcmp(evento, "PERIODICO") == 0;
}

void enqueueEvent(const char* evento) {
  if (isPeriodicEvent(evento)) return;

  // Se a fila encher, descarta o evento mais antigo para manter os mais recentes.
  if (eventCount >= EVENT_QUEUE_SIZE) {
    eventTail = (eventTail + 1) % EVENT_QUEUE_SIZE;
    eventCount--;
    Serial.println("[FILA] Cheia. Evento mais antigo descartado.");
  }

  strncpy(eventQueue[eventHead], evento, EVENT_NAME_SIZE - 1);
  eventQueue[eventHead][EVENT_NAME_SIZE - 1] = '\0';
  eventHead = (eventHead + 1) % EVENT_QUEUE_SIZE;
  eventCount++;

  Serial.print("[FILA] Evento guardado offline: ");
  Serial.println(evento);
}

bool dequeueEvent(char* destino) {
  if (eventCount == 0) return false;

  strncpy(destino, eventQueue[eventTail], EVENT_NAME_SIZE);
  destino[EVENT_NAME_SIZE - 1] = '\0';
  eventTail = (eventTail + 1) % EVENT_QUEUE_SIZE;
  eventCount--;
  return true;
}

String macToString(byte macAddr[]) {
  char buf[20];
  sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
  return String(buf);
}

String obterDataHora() {
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    timeClient.update();
    time_t t = timeClient.getEpochTime();
    if (t > 100000) {
      struct timeval tv = { .tv_sec = t, .tv_usec = 0 };
      settimeofday(&tv, NULL);
    }
  }

  if (getLocalTime(&timeinfo) && timeinfo.tm_year >= 120) {
    char dataHora[25];
    strftime(dataHora, sizeof(dataHora), "%d/%m/%Y %H:%M:%S", &timeinfo);
    return String(dataHora);
  }

  return String("SEM_NTP_UPTIME_") + String(millis() / 1000);
}

const char* calcularTurnoAtual() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return "SEM_HORA";

  int h = timeinfo.tm_hour;
  if (h >= 6 && h < 14) return "A";
  if (h >= 14 && h < 22) return "B";
  return "C";
}

bool montarJson(const char* evento, char* buffer, size_t bufferSize) {
  JsonDocument doc;

  String dataHora = obterDataHora();
  String macStr = macToString(mac);

  // Campos novos padronizados.
  doc["device_id"] = device_id;
  doc["event"] = evento;
  doc["timestamp"] = dataHora;
  doc["mac"] = macStr;
  doc["ip"] = ip.toString();
  doc["turno_atual"] = calcularTurnoAtual();
  doc["mqtt_queue_pending"] = eventCount;

  JsonObject sensors = doc["sensors"].to<JsonObject>();
  sensors["urdimento"] = estadoUrdimento;
  sensors["trama"] = estadoTrama;
  sensors["enrolamento_fita"] = estadoEnrolamento;
  sensors["stop"] = estadoStop;
  sensors["start"] = estadoStart;
  sensors["manutencao"] = estadoManutencao;

  JsonObject production = doc["production"].to<JsonObject>();
  production["turno_a"] = cont_producao_A;
  production["turno_b"] = cont_producao_B;
  production["turno_c"] = cont_producao_C;

  // Campos antigos mantidos temporariamente para não quebrar o HTML atual.
  doc["Evento"] = evento;
  doc["Data_Hora"] = dataHora;
  doc["ID_Máquina"] = device_id;
  doc["MAC"] = macStr;
  doc["S_Urdimento"] = estadoUrdimento;
  doc["S_trama"] = estadoTrama;
  doc["S_Enrolamento_Fita"] = estadoEnrolamento;
  doc["BT_Stop"] = estadoStop;
  doc["BT_Start"] = estadoStart;
  doc["BT_Manutenção"] = estadoManutencao;
  doc["Cont_A"] = cont_producao_A;
  doc["Cont_B"] = cont_producao_B;
  doc["Cont_C"] = cont_producao_C;

  size_t n = serializeJson(doc, buffer, bufferSize);
  return n > 0 && n < bufferSize;
}

void publicarDados(const char* evento) {
  if (!client.connected()) {
    enqueueEvent(evento);
    return;
  }

  char buffer[1536];
  if (!montarJson(evento, buffer, sizeof(buffer))) {
    Serial.println("[MQTT] Erro ao montar JSON. Buffer insuficiente.");
    return;
  }

  bool result = client.publish(topic_telemetry, buffer);
  if (!result) {
    Serial.println("[MQTT] Falha ao publicar. Evento colocado na fila.");
    enqueueEvent(evento);
    return;
  }

  Serial.print("[MQTT] Publicado: ");
  Serial.println(evento);
}

void flushEventQueue() {
  if (!client.connected() || eventCount == 0) return;

  char evento[EVENT_NAME_SIZE];
  int enviados = 0;
  int limiteEnvio = eventCount; // evita loop infinito caso uma publicação falhe e volte para a fila

  while (client.connected() && enviados < limiteEnvio && dequeueEvent(evento)) {
    publicarDados(evento);
    enviados++;
    delay(20); // pequeno respiro para o broker
  }

  if (enviados > 0) {
    Serial.print("[FILA] Eventos reenviados: ");
    Serial.println(enviados);
  }
}

void checa_rede() {
  Serial.println("Checando hardware Ethernet...");

  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("[ERRO] W5500 ausente ou com defeito.");
    return;
  }

  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("[REDE] Cabo desconectado.");
  } else {
    Serial.print("[REDE] Cabo conectado. IP: ");
    Serial.println(ip);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("[MQTT] Comando recebido em ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(message);

  if (String(topic) == topic_command) {
    if (message == "PING") {
      publicarDados("CMD_PING_RESPONSE");
    } else if (message == "RESET_CONTADORES") {
      resetarContadores();
      publicarDados("CMD_RESET_CONTADORES");
    } else if (message == "STATUS") {
      publicarDados("CMD_STATUS_RESPONSE");
    }
  }
}

void reconnect() {
  static unsigned long ultimaTentativa = 0;

  if (client.connected()) return;
  if (Ethernet.linkStatus() == LinkOFF) return;

  unsigned long agora = millis();
  if (agora - ultimaTentativa < 5000) return;

  ultimaTentativa = agora;
  Serial.print("[MQTT] Tentando conectar...");

  if (client.connect(device_id, mqtt_user, mqtt_pass, topic_status, 1, true, "offline")) {
    Serial.println(" conectado.");
    client.publish(topic_status, "online", true);
    client.subscribe(topic_command);
    publicarDados("MQTT_CONNECTED");
    flushEventQueue();
  } else {
    Serial.print(" falhou, rc=");
    Serial.println(client.state());
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);

  pinMode(W5500_RST_PIN, OUTPUT);
  digitalWrite(W5500_RST_PIN, LOW);
  delay(200);
  digitalWrite(W5500_RST_PIN, HIGH);
  delay(1000);

  Ethernet.init(ETH_SPI_SCS);
  Ethernet.begin(mac, ip, dns, gateway, subnet);

  Serial.println("\n--- Programa Coletor de Dados MQTT - V2 SEGURA ---");

  configurarPinos();
  inicializarSensores();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  client.setBufferSize(1536);

  timeClient.begin();
  setenv("TZ", "<-03>3", 1);
  tzset();

  // Tenta NTP, mas não trava o boot caso a rede esteja fora.
  if (Ethernet.linkStatus() == LinkON) {
    for (int i = 0; i < 3; i++) {
      if (timeClient.forceUpdate()) {
        time_t t = timeClient.getEpochTime();
        struct timeval tv = { .tv_sec = t, .tv_usec = 0 };
        settimeofday(&tv, NULL);
        Serial.println("[NTP] Hora sincronizada.");
        break;
      }
      delay(500);
    }
  }

  checa_rede();
  reconnect();
  Serial.println("Sistema iniciado. Sensores ativos mesmo sem rede.");
}

void loop() {
  atualizarStatusREDE(client);
  gerenciarTempoPiscaD4();

  monitorarSensorParaPisca(0, estadoUrdimento, client);
  monitorarSensorParaPisca(1, estadoTrama, client);
  monitorarSensorParaPisca(2, estadoEnrolamento, client);
  monitorarSensorParaPisca(3, estadoStop, client);
  monitorarSensorParaPisca(4, estadoStart, client);
  monitorarSensorParaPisca(5, estadoManutencao, client);
  monitorarSensorParaPisca(6, estadoBotao, client);

  if (!client.connected()) {
    reconnect();
  } else {
    client.loop();
    flushEventQueue();
  }

  verificarResetDiario();
  processarSensores(client.connected());
  salvarPendenciasFlashSeNecessario();

  unsigned long now = millis();
  if (now - lastMsgTime > interval) {
    lastMsgTime = now;
    publicarDados("PERIODICO");
  }

  if (now - lastNetworkCheck > networkCheckInterval) {
    lastNetworkCheck = now;
    checa_rede();
  }
}
