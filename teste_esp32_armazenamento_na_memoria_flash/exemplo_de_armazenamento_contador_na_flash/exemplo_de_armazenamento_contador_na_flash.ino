#include <Preferences.h> // Biblioteca para gravar na flash

Preferences preferences; // Cria uma instância da biblioteca

const int pinoBotao = 25;   // GPIO onde o botão está conectado
int contador = 0;          // Variável do contador
bool ultimoEstado = LOW;  // Estado anterior do botão

void setup() {
  Serial.begin(115200);
  pinMode(pinoBotao, INPUT_PULLUP); // Configura botão com resistor interno

  // Abre a "pasta" de armazenamento chamada "armazena_cont" (falso = leitura e escrita)
  preferences.begin("armazena_cont", false);

  // Recupera o valor salvo na chave "cont". Se não existir, inicia em 0.
  contador = preferences.getInt("cont", 0);

  Serial.print("Contador recuperado da memória: ");
  Serial.println(contador);
}

void loop() {
  bool estadoAtual = digitalRead(pinoBotao);

  // Detecta a transição de solto (HIGH) para pressionado (LOW)
  if (ultimoEstado == LOW && estadoAtual == HIGH) {
    delay(50); // Debounce simples para evitar leituras falsas
    contador++;
    
    Serial.print("Novo valor: ");
    Serial.println(contador);

    // Grava o novo valor na memória flash sob a chave "cont"
    preferences.putInt("cont", contador);
  }

  ultimoEstado = estadoAtual;
}