#ifndef VELOC_H
#define VELOC_H

#include <Arduino.h>

class LerVeloc {
private:
    unsigned long lastTime1, lastTime2, lastTime3;
    float veloc;
    int pinInput;
    bool pulseOn;
   
public:
    bool startBorda;
    LerVeloc(int pin) : lastTime1(0), lastTime2(0), lastTime3(0), startBorda(1), pinInput(pin), veloc(0), pulseOn(1) {}
    void begin(){
        pinMode(pinInput, INPUT_PULLDOWN);
    }
    float getVeloc(){
        // Leitura da velocidade
        if(digitalRead(pinInput) && pulseOn && (millis() - lastTime2 > 3)){
            if(startBorda){
                lastTime1 = millis();
                startBorda = 0;
            }
            else{
                veloc = float(millis() - lastTime1)*0.001; //Tempo entre bordas de subida em segundos
                veloc = 1.0/veloc; //Frequencia em Hz
                startBorda = 1;
            }
            lastTime3 = millis(); 
            pulseOn = 0;
        }
        else if(!digitalRead(pinInput)){
            lastTime2 = millis();
            pulseOn = 1;
        }
        if(millis() - lastTime3 >= 4010) veloc = 0.00; //Zera após 4.01 segundos (configuração do equipamento)
        return veloc*20.909090; //roundf(veloc*209.09090) / 10;
    }
};

#endif