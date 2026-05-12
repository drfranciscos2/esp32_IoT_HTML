#ifndef __PIN_MAP__
#define __PIN_MAP__

#include "Arduino.h"

// ENTRADAS

#define BT_01   13 
#define BT_02   12 
#define BT_03   14 
#define BT_04   27 
#define BT_05   26 
#define BT_06   25 
#define S_PULSO 33 

// SAIDAS

#define LED_NET     2           //Feedback rede
#define LED_SEND    4           //Feedback plataforma
#define LED_INPUT   21          //Feedback botão

// ETHERNET

#define CS      5
#define CLK     18
#define MISO    19
#define MOSI    23
#define REST    22  

#define port_server  80

// RS485
#define DE_RE  15
#define DI     16
#define RO     17    


 // Credenciais de Acesso

#define correctUsername  "admin"
#define correctPassword  "1234"




#endif