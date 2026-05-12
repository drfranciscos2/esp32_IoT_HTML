# Firmware - Stick Elasticos

# Requisitos

 * Comunicação devera ser feita por MQTT - Envio para um broker definido no servidor Realize
 * A conexão com internet deverá ser feita pelo cabo Ethernet
 * A placa deverá coletar data e hora da rede de internet.
 * A placa deverá ler 6 botãoes e 1 sensor de pulso
 * A placa deverá acionar 3 leds de forma externa 
 * Os possui funcionalidades de (LED 1: Conectado a Internet, LED 2: Comunicação Com a plataforma, LED 3: Confirmação de comando)
 
 * Regra de funcionamento dos LED's:
        * Conectou na rede ligou LED 1, perdeu conexão apaga o LED
        * Recebeu confirmação de da plataforma, LED 2 pisca durante 500 ms
        * Recebeu comando dos botões LED 3, pisca durante 500 ms

 * Pacote a ser enviado:
    {
        "id_pct" : "String"
        "timer(data e hora)" : "String"
        "BT-1 = Enrolamento de Fita": "String"
        "BT-2 = Ajuste de Cor": "String"
        "BT-3 = Produção": "String"
        "BT-4 = Manutenção": "String"
        "BT-5 = Limpeza de maquina": "String"
        "BT-6 = Outros" : "String"
        "IN7 = Sensor de Pulso" : "String"
    }