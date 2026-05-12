
//instancias e inicialização

#ifndef CONFIG_BOTOES_H
#define CONFIG_BOTOES_H

#include "BotaoESP32.h"

// Instâncias manuais para os pinos solicitados
BotaoESP32 b36(36), b39(39), b32(32), b33(33), b25(25), b26(26), b27(27), b14(14);
// observação: pinos 36 e 39 não possuem resistores de pull-up internos
// se usar acrescentar resistores ligados ao 3,3V 
// Inicialização dos botoes
void inicializarTodosBotoes() {
    b36.iniciar();
    b39.iniciar();
    b32.iniciar();
    b33.iniciar();
    b25.iniciar();
    b26.iniciar();
    b27.iniciar();
    b14.iniciar();
}

#endif