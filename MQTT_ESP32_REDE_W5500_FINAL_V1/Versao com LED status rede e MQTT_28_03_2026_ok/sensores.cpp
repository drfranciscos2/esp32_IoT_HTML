#include "Sensores.h"
#include "pinConfig.h"
#include <Preferences.h>

Preferences prefs;

// Inicialização das variáveis
int estadoBotao = 0;
uint32_t cont_producao_A = 0;
uint32_t cont_producao_B = 0;
uint32_t cont_producao_C = 0;

extern void publicarDados(const char* evento);

void inicializarSensores() {
    prefs.begin("producao", false);
    cont_producao_A = prefs.getUInt("contA", 0);
    cont_producao_B = prefs.getUInt("contB", 0);
    cont_producao_C = prefs.getUInt("contC", 0);
    prefs.end();
}

void resetarContadores() {
    prefs.begin("producao", false);
    cont_producao_A = 0; cont_producao_B = 0; cont_producao_C = 0;
    prefs.putUInt("contA", 0); prefs.putUInt("contB", 0); prefs.putUInt("contC", 0);
    prefs.end();
    Serial.println(">>> Contadores Resetados (05:59) <<<");
}

void processarSensores() {
    atualizarPinos(); // Atualiza os objetos Bounce2 (BT_1 a BT_8)

    // --- LÓGICA BOTÃO 25 (BT_5) ---
    if (BT_5.rose()) { estadoBotao = 1; digitalWrite(SAIDA_LED, HIGH); publicarDados("BOTAO_PRESSIONADO"); }
    if (BT_5.fell()) { estadoBotao = 0; digitalWrite(SAIDA_LED, LOW); publicarDados("BOTAO_SOLTO"); }

    //  CONTADOR ENTRADA 26 
    if (BT_6.fell()) { 
        struct tm timeinfo;
        if(!getLocalTime(&timeinfo)) return;

        int h = timeinfo.tm_hour;
        int m = timeinfo.tm_min;
        int s = timeinfo.tm_sec;

        // Reset Diário às 05:59:00
        if (h == 5 && m == 59 && s == 0) { resetarContadores(); return; }

        prefs.begin("producao", false);
        // Turno A: 06:00 as 13:59
        if (h >= 6 && h < 14) {
            cont_producao_A++;
            prefs.putUInt("contA", cont_producao_A);
        } 
        // Turno B: 14:00 as 21:59
        else if (h >= 14 && h < 22) {
            cont_producao_B++;
            prefs.putUInt("contB", cont_producao_B);
        } 
        // Turno C: 22:00 em diante até 05:58
        else {
            cont_producao_C++;
            prefs.putUInt("contC", cont_producao_C);
        }
        prefs.end();
        publicarDados("INCREMENTO_PRODUCAO");
    }
}