#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <Ethernet.h>
#include <PubSubClient.h>

class MQTTClient {
public:
    MQTTClient(const char* mqttServer, int mqttPort, const char* mqttUser, const char* mqttPassword, const uint8_t* macAddress);
    void begin();
    void loop();
    bool publish(const char* topic, const char* message);
    bool isConnected();

private:
    const char* _mqttServer;
    int _mqttPort;
    const char* _mqttUser;
    const char* _mqttPassword;
    const uint8_t* _macAddress;
    String _clientId;
    
    EthernetClient _ethernetClient;
    PubSubClient _mqttClient;

    void connectMQTT();
    void generateClientId();
    void callback(char* topic, byte* payload, unsigned int length);
    void printMQTTError(int state);
};

#endif