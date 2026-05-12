#ifndef HORADATANTP_H
#define HORADATANTP_H

#include <Arduino.h>
#include <Preferences.h>
#include "time.h"

class Gerenciador {
  private:
    Preferences preferences;
    int contador;
    bool jaResetouHoje;
    unsigned long tempoAnteriorSerial;

  public:
    Gerenciador();
    void iniciar();
    void atualizar();
    void incrementar();
    void zerarContador();
    void verificarResetAutomatico(struct tm* timeinfo);
    void exibirStatus(struct tm* timeinfo);
    
    // Getters para os pinos ou estados se necessário
    int getContador() { return contador; }
};

#endif