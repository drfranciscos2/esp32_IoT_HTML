#include "MQTT/MQTT.h"

MQTTClient::MQTTClient(const char* mqttServer, int mqttPort, const char* mqttUser, const char* mqttPassword, const uint8_t* macAddress)
    : _mqttServer(mqttServer), _mqttPort(mqttPort), _mqttUser(mqttUser), 
      _mqttPassword(mqttPassword), _macAddress(macAddress), 
      _mqttClient(_ethernetClient) {
    
    generateClientId();
    _mqttClient.setServer(_mqttServer, _mqttPort);
    _mqttClient.setCallback([this](char* topic, byte* payload, unsigned int length) {
        this->callback(topic, payload, length);
    });
    _mqttClient.setSocketTimeout(15);
    _mqttClient.setKeepAlive(60);
}

void MQTTClient::generateClientId() {
    _clientId = "Coletor de Dados - ";
    for (int i = 0; i < 6; i++) {
        if (_macAddress[i] < 0x10) {
            _clientId += "0";
        }
        _clientId += String(_macAddress[i], HEX);
    }
    _clientId.toUpperCase();
}

void MQTTClient::begin() {
    Serial.print("Client ID gerado: ");
    Serial.println(_clientId);
    connectMQTT();
}

void MQTTClient::loop() {
    if (!_mqttClient.connected()) {
        connectMQTT();
    }
    _mqttClient.loop();
}

bool MQTTClient::publish(const char* topic, const char* message) {
    if (!_mqttClient.connected()) {
        Serial.println("Cliente desconectado. Tentando reconectar...");
        connectMQTT();
    }
    
    if (_mqttClient.connected()) {
        bool success = _mqttClient.publish(topic, message);
        if (success) {
            Serial.println("Mensagem publicada com sucesso");
        } else {
            Serial.println("Falha ao publicar mensagem");
        }
        return success;
    }
    return false;
}

bool MQTTClient::isConnected() {
    return _mqttClient.connected();
}

void MQTTClient::connectMQTT() {
    int tentativas = 0;
    const int maxTentativas = 3;
    
    while (!_mqttClient.connected() && tentativas < maxTentativas) {
        Serial.print("Tentativa ");
        Serial.print(tentativas + 1);
        Serial.print(" de ");
        Serial.print(maxTentativas);
        Serial.println(" - Conectando ao MQTT...");

        if (_mqttClient.connect(_clientId.c_str(), _mqttUser, _mqttPassword)) {
            Serial.println("Conectado ao broker MQTT!");
            Serial.print("Client ID: ");
            Serial.println(_clientId);
            return;
        }

        Serial.print("Falha na conexão (rc=");
        Serial.print(_mqttClient.state());
        Serial.print("): ");
        printMQTTError(_mqttClient.state());
        
        tentativas++;
        if (tentativas < maxTentativas) {
            delay(5000);
        }
    }
    
    Serial.println("Não foi possível conectar ao broker após várias tentativas");
}

void MQTTClient::callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Mensagem recebida [");
    Serial.print(topic);
    Serial.print("]: ");
    
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

void MQTTClient::printMQTTError(int state) {
    switch (state) {
        case MQTT_CONNECTION_TIMEOUT:     Serial.println("Timeout de conexão"); break;
        case MQTT_CONNECTION_LOST:        Serial.println("Conexão perdida"); break;
        case MQTT_CONNECT_FAILED:         Serial.println("Falha na conexão"); break;
        case MQTT_DISCONNECTED:           Serial.println("Desconectado"); break;
        case MQTT_CONNECT_BAD_PROTOCOL:   Serial.println("Protocolo inválido"); break;
        case MQTT_CONNECT_BAD_CLIENT_ID:  Serial.println("Client ID inválido"); break;
        case MQTT_CONNECT_UNAVAILABLE:    Serial.println("Broker indisponível"); break;
        case MQTT_CONNECT_BAD_CREDENTIALS:Serial.println("Credenciais inválidas"); break;
        case MQTT_CONNECT_UNAUTHORIZED:   Serial.println("Não autorizado"); break;
        default:                          Serial.println("Erro desconhecido");
    }
}