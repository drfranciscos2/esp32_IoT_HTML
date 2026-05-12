#include "Sensores.h"
#include "pinConfig.h"
#include <Preferences.h>
#include <time.h>

Preferences prefs;

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

static int ultimaDataReset = -1;
static bool flashDirty = false;
static unsigned long ultimaGravacaoFlash = 0;
static uint16_t incrementosPendentesFlash = 0;

const unsigned long FLASH_SAVE_INTERVAL_MS = 30000;
const uint16_t FLASH_SAVE_EVERY_N_INCREMENTS = 25;

int dataAtualYYYYMMDD() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) return -1;
    if (timeinfo.tm_year < 120) return -1;
    int ano = timeinfo.tm_year + 1900;
    int mes = timeinfo.tm_mon + 1;
    int dia = timeinfo.tm_mday;
    return (ano * 10000) + (mes * 100) + dia;
}

void marcarFlashSuja(bool incremento = false) {
    flashDirty = true;
    if (incremento) incrementosPendentesFlash++;
}

void salvarTudoNaFlash() {
    prefs.begin("producao", false);
    prefs.putUInt("contA", cont_producao_A);
    prefs.putUInt("contB", cont_producao_B);
    prefs.putUInt("contC", cont_producao_C);
    prefs.putInt("uData", ultimaDataReset);
    prefs.putInt("stStart", estadoStart);
    prefs.putInt("stUrd", estadoUrdimento);
    prefs.putInt("stTrama", estadoTrama);
    prefs.putInt("stEnrol", estadoEnrolamento);
    prefs.putInt("stManut", estadoManutencao);
    prefs.putInt("stStop", estadoStop);
    prefs.end();
    flashDirty = false;
    incrementosPendentesFlash = 0;
    ultimaGravacaoFlash = millis();
    Serial.println("[FLASH] Estado salvo.");
}

void salvarPendenciasFlashSeNecessario() {
    if (!flashDirty) return;
    unsigned long agora = millis();
    bool tempoAtingido = (agora - ultimaGravacaoFlash >= FLASH_SAVE_INTERVAL_MS);
    bool limiteIncrementos = (incrementosPendentesFlash >= FLASH_SAVE_EVERY_N_INCREMENTS);
    if (tempoAtingido || limiteIncrementos) salvarTudoNaFlash();
}

void inicializarSensores() {
    prefs.begin("producao", false);
    cont_producao_A = prefs.getUInt("contA", 0);
    cont_producao_B = prefs.getUInt("contB", 0);
    cont_producao_C = prefs.getUInt("contC", 0);
    ultimaDataReset = prefs.getInt("uData", -1);
    if (ultimaDataReset == -1 && prefs.getInt("uDia", -1) != -1) {
        Serial.println("[FLASH] Formato antigo de reset por dia do mes encontrado. Sera atualizado no proximo reset.");
    }
    estadoStart = prefs.getInt("stStart", 0);
    estadoUrdimento = prefs.getInt("stUrd", 0);
    estadoTrama = prefs.getInt("stTrama", 0);
    estadoEnrolamento = prefs.getInt("stEnrol", 0);
    estadoManutencao = prefs.getInt("stManut", 0);
    estadoStop = prefs.getInt("stStop", 0);
    prefs.end();
    ultimaGravacaoFlash = millis();
    Serial.println("[SENSORES] Contadores e estados carregados da Flash.");
}

void resetarContadores() {
    int dataAtual = dataAtualYYYYMMDD();
    cont_producao_A = 0;
    cont_producao_B = 0;
    cont_producao_C = 0;
    ultimaDataReset = dataAtual;
    salvarTudoNaFlash();
    Serial.println(">>> Contadores resetados e data completa salva! <<<");
}

void verificarResetDiario() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) return;
    if (timeinfo.tm_year < 120) return;
    int h = timeinfo.tm_hour;
    int m = timeinfo.tm_min;
    int dataAtual = dataAtualYYYYMMDD();
    if (dataAtual == -1) return;
    if ((h > 5 || (h == 5 && m >= 59)) && ultimaDataReset != dataAtual) {
        resetarContadores();
        publicarDados("RESET_DIARIO_CONTADORES");
    }
}

void atualizarEstadoComPersistencia(int &estado, int novoValor) {
    if (estado != novoValor) {
        estado = novoValor;
        marcarFlashSuja(false);
    }
}

void incrementarContadorTurnoAtual() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println(">>> CONTAGEM IGNORADA: horario nao sincronizado <<<");
        return;
    }
    int h = timeinfo.tm_hour;
    if (h >= 6 && h < 14) cont_producao_A++;
    else if (h >= 14 && h < 22) cont_producao_B++;
    else cont_producao_C++;
    marcarFlashSuja(true);
}

void processarSensores(bool mqttConectado) {
    atualizarPinos();

    if (BT_5.rose()) { atualizarEstadoComPersistencia(estadoStart, 1); publicarDados("START_PRESSIONADO"); }
    if (BT_5.fell()) { atualizarEstadoComPersistencia(estadoStart, 0); publicarDados("START_SOLTO"); }

    if (BT_1.rose()) { atualizarEstadoComPersistencia(estadoUrdimento, 1); publicarDados("URDIMENTO_ATIVO"); }
    if (BT_1.fell()) { atualizarEstadoComPersistencia(estadoUrdimento, 0); publicarDados("URDIMENTO_INATIVO"); }

    if (BT_2.rose()) { atualizarEstadoComPersistencia(estadoTrama, 1); publicarDados("TRAMA_ATIVA"); }
    if (BT_2.fell()) { atualizarEstadoComPersistencia(estadoTrama, 0); publicarDados("TRAMA_INATIVA"); }

    if (BT_3.rose()) { atualizarEstadoComPersistencia(estadoEnrolamento, 1); publicarDados("ENROLAMENTO_FITA_ATIVO"); }
    if (BT_3.fell()) { atualizarEstadoComPersistencia(estadoEnrolamento, 0); publicarDados("ENROLAMENTO_FITA_INATIVO"); }

    if (BT_7.rose()) { atualizarEstadoComPersistencia(estadoManutencao, 1); publicarDados("MANUTENCAO_ATIVA"); }
    if (BT_7.fell()) { atualizarEstadoComPersistencia(estadoManutencao, 0); publicarDados("MANUTENCAO_INATIVA"); }

    if (BT_8.rose()) { atualizarEstadoComPersistencia(estadoStop, 1); publicarDados("STOP_PRESSIONADO"); }
    if (BT_8.fell()) { atualizarEstadoComPersistencia(estadoStop, 0); publicarDados("STOP_SOLTO"); }

    bool travado = (estadoEnrolamento || estadoTrama || estadoUrdimento || estadoManutencao);
    if (BT_6.fell()) {
        if (travado) {
            Serial.println(">>> CONTAGEM BLOQUEADA: sensor de trava ativo <<<");
            publicarDados("CONTAGEM_BLOQUEADA_SENSOR_ATIVO");
            return;
        }
        incrementarContadorTurnoAtual();
        publicarDados("INCREMENTO_PRODUCAO");
    }
}
