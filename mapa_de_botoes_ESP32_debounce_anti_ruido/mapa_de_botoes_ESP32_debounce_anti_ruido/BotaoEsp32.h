
//define a classe e a estrutura para cada pino 


#ifndef BOTAO_ESP32_H
#define BOTAO_ESP32_H

#include <Arduino.h>

class BotaoESP32 {
private:
    uint8_t pino;
    bool estadoUltimo = LOW;
    bool estadoDebounced = LOW;
    unsigned long tempoUltimoDebounce = 0;
    static const unsigned long delayDebounce = 50; 

public:
    BotaoESP32(uint8_t p) : pino(p) {}
    
    void iniciar() { pinMode(pino, INPUT_PULLUP); }

    // Retorna true apenas no instante em que o botão é pressionado
    bool pressionado() {
        bool leitura = digitalRead(pino);
        bool detectado = false;

        if (leitura != estadoUltimo) {
            tempoUltimoDebounce = millis();
        }

        if ((millis() - tempoUltimoDebounce) > delayDebounce) {
            if (leitura != estadoDebounced) {
                estadoDebounced = leitura;
                if (estadoDebounced == HIGH) {
                    detectado = true;
                }
            }
        }
        estadoUltimo = leitura;
        return detectado;
    }
};

#endif