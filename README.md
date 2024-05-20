
## Estrutura do projeto

O projeto é composto por:
- 2 controladores com conexão WiFi, na qual serão programados atraves da IDE do Arduino;
- Servidor de protocolo MQTT para realizar a ponte entre os controladores de leitura e de controle, além do Node-RED;
- Servidor Node-RED para exibir os valores da leitura do sensor, além de controlar as variáveis de Temperatura ideal, Umidade ideal e dos switchs do ar condicionado e do umidificador;

## Hardware 

- Protoboard 400 pontos

1. Controlador de Leitura do Ambiente:
- Placa DOIT ESP32 DevKit - WiFi / Bluetooth
- Sensor de temperatura e umidade do ar DHT11

2. Controlador Eletronicos:
- Placa DOIT ESP32 DevKit - WiFi / Bluetooth
- 2 x Relés 5v


## Funcionamento

1.[Controlador de Leitura do Ambiente](https://github.com/glopess/automacao_casa/blob/main/leitura-ambiente.ino):
- Inicia com os valores default das variaveis `Temperatura Ideal` (25) e `Umidade Ideal` (45);
- Realiza a assinatura dos topicos `tempIdeal` e `umidIdeal` do MQTT Broker para atualizar as variaveis sempre que uma mensagem dos respectivos topicos for recebida;
- A cada 7 segundos realiza a leitura de temperatura e de umidade do sensor DHT11;
- Publica nos tópicos `umidade` e `temperatura` os valores obtidos;
- Se temperatura do ambiente for maior que variavel `Temperatura Ideal`, publica no topico de `switchAr` o comando para ligar;
- Se umidade do ambiente for menor que variavel `Umidade Ideal`, publica no topico de `switchUmid` o comando para ligar;
- Se temperatura do ambiente for menor que variavel `Temperatura Ideal` depois de 10 leituras seguidas, publica no topico de `switchAr` o comando para desligar;
- Se umidade do ambiente for maior que variavel `Umidade Ideal` depois de 10 leituras seguidas, publica no topico de `switchUmid` o comando para desligar;

2. [Controlador Eletronicos](https://github.com/glopess/automacao_casa/blob/main/controlador-eletronicos.ino):
- Realiza a assinatura dos topicos `switchAr` e `switchUmid` do MQTT Broker para atualizar o estados dos respectivos relés sempre que uma mensagem for recebida;

3. Dashboard Node-RED:
- Recebe e exibe os valores recebidos dos topicos `umidade`, `temperatura`, `umidIdeal`, `tempIdeal`, `switchAr`,`switchUmid`;
- Publica nos tópicos  `umidIdeal`, `tempIdeal` para enviar ao Controlador de Leitura os valores atualizados;
- Publica nos tópicos `switchAr`,`switchUmid` para enviar ao Controlador Eletronicos os comandos de ligar/desligar do ar e do umidificador;