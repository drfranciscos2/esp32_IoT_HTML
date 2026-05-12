//Bibliotecas
#include <HTTPClient.h>
#include <DNSServer.h> 
#include <WebServer.h>
#include <WiFiManager.h>

WiFiManager wifiManager;//Objeto de manipulação do wi-fi

void setup(){
  
  Serial.begin(115200);
  Serial.println();

  //Definição dos pinos
  pinMode(13,INPUT);
  pinMode(26,OUTPUT); 
  pinMode(32,OUTPUT);

  //LEDs apagados
  digitalWrite(26,LOW);
  digitalWrite(32,LOW);
  
  //callback para quando entra em modo de configuração AP
  wifiManager.setAPCallback(configModeCallback); 
  //callback para quando se conecta em uma rede, ou seja, quando passa a trabalhar em modo estação
  wifiManager.setSaveConfigCallback(saveConfigCallback); 

}

void loop() {

   //Se o botão for pressionado
   if (digitalRead(13) == LOW) {
      Serial.println("Abertura Portal"); //Abre o portal
      digitalWrite(26,HIGH); //Acende LED Vermelho
      digitalWrite(32,LOW);
      wifiManager.resetSettings();       //Apaga rede salva anteriormente
      if(!wifiManager.startConfigPortal("ESP32-CONFIG", "123456") ){ //Nome da Rede e Senha gerada pela ESP
        Serial.println("Falha ao conectar"); //Se caso não conectar na rede mostra mensagem de falha
        delay(2000);
        ESP.restart(); //Reinicia ESP após não conseguir conexão na rede
      }
      else{       //Se caso conectar 
        Serial.println("Conectado na Rede!!!");
        ESP.restart(); //Reinicia ESP após conseguir conexão na rede 
      }
   }

   if(WiFi.status()== WL_CONNECTED){ //Se conectado na rede
      digitalWrite(32,HIGH); //Acende LED AZUL
   }
   else{ //se não conectado na rede
      digitalWrite(32,LOW); //Apaga LED AZUL
      //Pisca LED Vermelho
      digitalWrite(26,HIGH);
      delay(500);
      digitalWrite(26,LOW);
      delay(500);
      wifiManager.autoConnect();//Função para se autoconectar na rede
   }  
}

//callback que indica que o ESP entrou no modo AP
void configModeCallback (WiFiManager *myWiFiManager) {  
  Serial.println("Entrou no modo de configuração");
  Serial.println(WiFi.softAPIP()); //imprime o IP do AP
  Serial.println(myWiFiManager->getConfigPortalSSID()); //imprime o SSID criado da rede
}

//Callback que indica que salvamos uma nova rede para se conectar (modo estação)
void saveConfigCallback () {
  Serial.println("Configuração salva");
}
