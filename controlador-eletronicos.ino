#include <iostream>
#include <WiFi.h>
#include <PubSubClient.h> 
#include "Esp32MQTTClient.h" 
#include "DHT.h" 

const char* ssid = "";
const char* password = "";
const char* mqtt_server = "192.168.1.110";

static const char* connectionString = "";
long lastTemp = 0;

int arRelayPin = 4;
int umdfRelayPin = 5;
int LED = 2;         

static bool hasIoTHub = false;

WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(arRelayPin, OUTPUT);
  pinMode(umdfRelayPin, OUTPUT);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Menssagem recebida[");
  Serial.print(topic);
  Serial.print("] ");
  char* str = "";
  str = reinterpret_cast<char*>(payload);

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);    
  }
  Serial.println();

  if(strcmp("switchAr",topic) == 0){
    switchAr((char)payload[0]);
    piscar(2);
  }

  if(strcmp("switchUmid",topic) == 0){
    switchUmid((char)payload[0]);
    piscar(2);
  }
}
void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Conectado a rede: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Conectado");
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());
}
void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Conectando ao MQTT...");
    
    if (client.connect("ESP32Controller"))
    {
      Serial.println("Conectado");      
      client.publish("event", "Controlador Conectado");      
      client.subscribe("switchAr");
      client.subscribe("switchUmid");
    }
    else
    {
      Serial.print("Erro:");
      Serial.print(client.state());
      Serial.println(" reconectando em 5 segundos");
      
      delay(5000);
    }
  }
}
void loop()
{

  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
  long now = millis();
}

void piscar(int x){
  int count = 0;
  while(x > count){
    digitalWrite(LED , HIGH);
    delay(300);
    digitalWrite(LED , LOW);
    delay(300);
    count++;
  }
}

void switchAr(char x){
  if(x == '0'){
    digitalWrite(arRelayPin, HIGH);
    Serial.println("Desligando Ar.");
  }else{
    digitalWrite(arRelayPin, LOW);
    Serial.println("Ligando Ar.");
  }
}

void switchUmid(char x){
  if(x == '0'){
    digitalWrite(umdfRelayPin, HIGH);
    Serial.println("Desligando Umidificador.");
  }else{
    digitalWrite(umdfRelayPin, LOW);
    Serial.println("Ligando Umidificador.");
  }
}