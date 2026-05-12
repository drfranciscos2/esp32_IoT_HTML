#include "Sensores.h"
#include "pinConfig.h"
#include <Preferences.h>

Preferences prefs;

// Inicialização das variáveis
//static int ultimoDiaZerado = -1; // Guarda o dia em que o reset ocorreu
int estadoBotao = 0;
int estadoUrdimento = 0;
int estadoTrama = 0;
int estadoEnrolamento = 0;
int estadoStop = 0;
int estadoStart = 0;
int estadoManutencao = 0;
uint32_t cont_producao_A = 0;
uint32_t cont_producao_B = 0;
uint32_t cont_producao_C = 0;

 

extern void publicarDados(const char* evento);


static int ultimoDiaZerado = -1; 
//---------------------------------adcionado--------------------------------------
void salvarEstadoBotaoFlash(const char* chave, int valor) {
    prefs.begin("producao", false);
    prefs.putInt(chave, valor);
    prefs.end();
}
//-----------------------------------------------------------------------

void inicializarSensores() {
    prefs.begin("producao", false);
    cont_producao_A = prefs.getUInt("contA", 0);
    cont_producao_B = prefs.getUInt("contB", 0);
    cont_producao_C = prefs.getUInt("contC", 0);
    
    // CARREGA O DIA DO ÚLTIMO RESET SALVO
    ultimoDiaZerado = prefs.getInt("uDia", -1); 
    
    // NOVIDADE: Recupera estados dos botões------adcionado--------------
    estadoStart = prefs.getInt("stStart", 0);
    estadoUrdimento = prefs.getInt("stUrd", 0);
    estadoTrama = prefs.getInt("stTrama", 0);
    estadoEnrolamento = prefs.getInt("stEnrol", 0);
    estadoManutencao = prefs.getInt("stManut", 0);
    estadoStop = prefs.getInt("stStop", 0);
    //----------------------------------------------------------
    prefs.end();
    Serial.println("  Dados e Estados dos botões carregados da Flash."); // <----adcionado
    Serial.print("Dados carregados. Ultimo reset no dia: ");
    Serial.println(ultimoDiaZerado);
}

void resetarContadores() {
    struct tm timeinfo;
    int diaAtual = -1;
    if (getLocalTime(&timeinfo)) {
        diaAtual = timeinfo.tm_mday;
    }

    prefs.begin("producao", false);
    cont_producao_A = 0; 
    cont_producao_B = 0; 
    cont_producao_C = 0;
    ultimoDiaZerado = diaAtual; // Atualiza a variável na RAM

    prefs.putUInt("contA", 0); 
    prefs.putUInt("contB", 0); 
    prefs.putUInt("contC", 0);
    prefs.putInt("uDia", ultimoDiaZerado); // SALVA O DIA DO RESET NA FLASH
    prefs.end();
    
    Serial.println(">>> Contadores Resetados e Dia do Reset Salvo! <<<");
}

void verificarResetDiario() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) return;

    // Se o ano for 1970, o NTP ainda não sincronizou. Não resete nada!
    if (timeinfo.tm_year < 120) return; 

    int h = timeinfo.tm_hour;
    int m = timeinfo.tm_min;
    int diaAtual = timeinfo.tm_mday;

    // Só reseta se: Passou das 05:59 E o dia atual for diferente do dia do último reset salvo
    if ((h > 5 || (h == 5 && m >= 59)) && ultimoDiaZerado != diaAtual) {            //h > 5            h==5 && >= 59
        resetarContadores();
    }
}
void processarSensores(bool mqttConectado) {   // (bool mqttConectado)
    atualizarPinos(); // Atualiza os objetos Bounce2 (BT_1 a BT_8)

  // --- LÓGICA BOTÃO START (BT_5 / Pino 25) ---
    if (BT_5.rose()) { estadoStart = 1; salvarEstadoBotaoFlash("stStart", 1); publicarDados("BOTAO_25_PRESSIONADO"); }
    if (BT_5.fell()) { estadoStart = 0; salvarEstadoBotaoFlash("stStart", 0); publicarDados("BOTAO_25_SOLTO"); }

    // --- LÓGICA URDIMENTO (BT_1 / Pino 13) ---
    if (BT_1.rose()) { estadoUrdimento = 1; salvarEstadoBotaoFlash("stUrd", 1); publicarDados("BOTAO_13_PRESSIONADO"); }
    if (BT_1.fell()) { estadoUrdimento = 0; salvarEstadoBotaoFlash("stUrd", 0); publicarDados("BOTAO_13_SOLTO"); }

    // --- LÓGICA TRAMA (BT_2 / Pino 12) ---
    if (BT_2.rose()) { estadoTrama = 1; salvarEstadoBotaoFlash("stTrama", 1); publicarDados("BOTAO_12_PRESSIONADO"); }
    if (BT_2.fell()) { estadoTrama = 0; salvarEstadoBotaoFlash("stTrama", 0); publicarDados("BOTAO_12_SOLTO"); }
    
    // --- LÓGICA ENROLAMENTO (BT_3 / Pino 14) ---
    if (BT_3.rose()) { estadoEnrolamento = 1; salvarEstadoBotaoFlash("stEnrol", 1); publicarDados("BOTAO_14_PRESSIONADO"); }
    if (BT_3.fell()) { estadoEnrolamento = 0; salvarEstadoBotaoFlash("stEnrol", 0); publicarDados("BOTAO_14_SOLTO"); }

    // --- LÓGICA MANUTENÇÃO (BT_7 / Pino 27) ---
    if (BT_7.rose()) { estadoManutencao = 1; salvarEstadoBotaoFlash("stManut", 1); publicarDados("BOTAO_27_PRESSIONADO"); }
    if (BT_7.fell()) { estadoManutencao = 0; salvarEstadoBotaoFlash("stManut", 0); publicarDados("BOTAO_27_SOLTO"); }

    // --- LÓGICA STOP (BT_8 / Pino 21) ---
    if (BT_8.rose()) { estadoStop = 1; salvarEstadoBotaoFlash("stStop", 1); publicarDados("BOTAO_21_PRESSIONADO"); }
    if (BT_8.fell()) { estadoStop = 0; salvarEstadoBotaoFlash("stStop", 0); publicarDados("BOTAO_21_SOLTO"); }
    


                                                                                                               
    
    // TRAVA O CONTADOR SE ALGUM SENSOR ESTIVER ATIVO ( NIVEL LOGICO 1)
    bool travado = (estadoEnrolamento || estadoTrama || estadoUrdimento || estadoManutencao);
                     
                    

    //  CONTADOR PINO  D26 
    if (BT_6.fell()) { 
        if (travado) {
            Serial.println(">>> CONTAGEM BLOQUEADA: Sensor Ativo Detectado <<<");
            return; // Aborta a função aqui, não incrementa nem salva nada
        }
        struct tm timeinfo;
        if(!getLocalTime(&timeinfo)) return;

        int h = timeinfo.tm_hour;
        int m = timeinfo.tm_min;
        int s = timeinfo.tm_sec;

        

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