#include <Bounce2.h>

// Definição dos pinos
const int PINO_1 = 12;
const int PINO_2 = 13;
const int PINO_3 = 14;
const int PINO_4 = 25;
const int PINO_5 = 26;
const int PINO_6 = 27;
const int LED_SAIDA = 2; // LED interno do ESP32 ou saída externa

// Instanciando os objetos Bounce
Bounce botao1 = Bounce();
Bounce botao2 = Bounce();
Bounce botao3 = Bounce();
Bounce botao4 = Bounce();
Bounce botao5 = Bounce();
Bounce botao6 = Bounce();

void setup() {
  Serial.begin(115200);

  // Configura a saída do LED
  pinMode(LED_SAIDA, OUTPUT);
  digitalWrite(LED_SAIDA, LOW); // Começa desligado

  // Configuração dos botões
  botao1.attach(PINO_1, INPUT_PULLUP);
  botao1.interval(25);

  botao2.attach(PINO_2, INPUT_PULLUP);
  botao2.interval(25);

  botao3.attach(PINO_3, INPUT_PULLUP);
  botao3.interval(25);

  botao4.attach(PINO_4, INPUT_PULLUP);
  botao4.interval(25);

  botao5.attach(PINO_5, INPUT_PULLUP);
  botao5.interval(25);

  botao6.attach(PINO_6, INPUT_PULLUP);
  botao6.interval(25);
}

void loop() {
  // Atualiza todos os botões
  botao1.update();
  botao2.update();
  botao3.update();
  botao4.update();
  botao5.update();
  botao6.update();

  // Lógica específica para o PINO_4 (Botão 4) usa .fell
  if (botao4.fell()) {
    digitalWrite(LED_SAIDA, LOW); // APAGA o LED
    Serial.println("Botão 4 pressionado -> LED APAGADO");
  }

  if (botao4.rose()) { // usa .rose
    digitalWrite(LED_SAIDA, HIGH);  // ACENDE o LED
    Serial.println("Botão 4 solto -> LED ACESO");
  }

  // Monitoramento dos outros botões (opcional)
  if (botao1.fell()) Serial.println("Botão 1 Pressionado");
  if (botao2.fell()) Serial.println("Botão 2 Pressionado");
  if (botao3.fell()) Serial.println("Botão 3 Pressionado");
  if (botao5.fell()) Serial.println("Botão 5 Pressionado");
  if (botao6.fell()) Serial.println("Botão 6 Pressionado");
}