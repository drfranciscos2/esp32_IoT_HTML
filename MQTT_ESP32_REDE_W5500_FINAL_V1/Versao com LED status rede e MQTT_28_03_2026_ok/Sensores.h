#ifndef SENSORES_H
#define SENSORES_H

#include <Bounce2.h>
#include "pinConfig.h"
#include <Arduino.h>

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