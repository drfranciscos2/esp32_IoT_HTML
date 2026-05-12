#ifndef SENSORES_H
#define SENSORES_H

#include <Bounce2.h>
#include "pinConfig.h"
#include <Arduino.h>

inline void gerenciarPiscaSemDelay(int estadoAtual) {
    // Variáveis estáticas: mantêm o valor na memória entre as chamadas da função
    static int ultimoEstadoBotao = -1; 
    static unsigned long tempoLigouLed = 0;
    static bool ledAceso = false;

    unsigned long tempoAtual = millis();

    // 1. Detecta se o estado do botão MUDOU (de 0 para 1 ou 1 para 0)
    if (estadoAtual != ultimoEstadoBotao) {
        digitalWrite(SAIDA_1, HIGH);
        tempoLigouLed = tempoAtual;
        ledAceso = true;
        ultimoEstadoBotao = estadoAtual;
        
        // Opcional: Serial.println("Mudança de estado - LED D4 Piscou");
    }

    // 2. Desliga o LED após 100ms (sem travar o código)
    if (ledAceso && (tempoAtual - tempoLigouLed >= 100)) {
        digitalWrite(SAIDA_1, LOW);
        ledAceso = false;
    }
}

// Declaração das funções que o arquivo principal chamará
void configurarBotao();
void processarBotao();
void resetarContadores();
void inicializarSensores();
void processarSensores(); 
// Extern para que o loop principal possa ler o estado se precisar
extern int estadoBotao; 
extern uint32_t cont_producao_A;
extern uint32_t cont_producao_B;
extern uint32_t cont_producao_C;

#endif