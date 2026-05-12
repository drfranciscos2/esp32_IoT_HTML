#include "HoraDataNTP.h"

Gerenciador::Gerenciador() {
    contador = 0;
    jaResetouHoje = false;
    tempoAnteriorSerial = 0;
}

void Gerenciador::iniciar() {
    preferences.begin("armazena_cont", false);
    contador = preferences.getInt("cont", 0);
    Serial.printf("Contador recuperado da Flash: %d\n", contador);
}

void Gerenciador::incrementar() {
    contador++;
    preferences.putInt("cont", contador);
    Serial.println("-> Incrementado e salvo!");
}

void Gerenciador::zerarContador() {
    contador = 0;
    preferences.putInt("cont", 0);
}

void Gerenciador::exibirStatus(struct tm* timeinfo) {
    if (millis() - tempoAnteriorSerial >= 1000) {
        tempoAnteriorSerial = millis();
        
        // Aumenta buffer para "DD/MM/AAAA HH:MM:SS" 
        char dataHoraFormatada[20]; 
        
        // %d = dia, %m = mês, %Y = ano com 4 dígitos
        strftime(dataHoraFormatada, sizeof(dataHoraFormatada), "%d/%m/%Y %H:%M:%S", timeinfo);
        
        Serial.printf("[%s] Contador: %d\n", dataHoraFormatada, contador);
        Serial.printf("[%s] ",dataHoraFormatada);

    }
}

void Gerenciador::verificarResetAutomatico(struct tm* timeinfo) {
    if (timeinfo->tm_hour == 6 && timeinfo->tm_min == 0 && !jaResetouHoje) {
        zerarContador();
        jaResetouHoje = true;
        Serial.println(">>> ALERTA: Memória zerada (06:00 AM)!");
    }
    if (timeinfo->tm_hour == 6 && timeinfo->tm_min == 1) {
        jaResetouHoje = false;
    }
}