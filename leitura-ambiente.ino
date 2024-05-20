#include <iostream>

#include <WiFi.h>
#include <PubSubClient.h> 
#include "Esp32MQTTClient.h" 
#include "DHT.h" 

const char* ssid = "";
const char* password = "";
const char* mqtt_server = "192.168.1.110";

#define DHTTYPE DHT11
#define DHTPIN 5
DHT dht(DHTPIN, DHTTYPE);

static const char* connectionString = "";
long lastTemp = 0;
int tempIdeal = 25;
int umidIdeal = 45;
long countLoopTemp = 0;
long countLoopUmid = 0;

int LED = 2;            
static bool hasIoTHub = false;

WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
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

  if(strcmp("tempIdeal",topic) == 0){
    Serial.print("Atualizando UmidIdeal. ");
    Serial.print(tempIdeal);
    Serial.print(" -> ");
    tempIdeal = atof(str);
    Serial.print(tempIdeal);
    Serial.println();
  }

  if(strcmp("umidIdeal",topic) == 0){
    Serial.print("Atualizando UmidIdeal.");
    Serial.print(umidIdeal);
    Serial.print(" -> ");
    umidIdeal = atof(str);
    Serial.print(umidIdeal);
    Serial.println();
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
    
    if (client.connect("ESP32LeituraAmbiente"))
    {
      Serial.println("Conectado");      
      client.publish("event", "Leitura ambiente Conectada");      
      client.subscribe("tempIdeal");
      client.subscribe("umidIdeal");
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

if (now - lastTemp > 7000) {
     lastTemp = now;

    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if(!isnan(t)){
      char TempString[8];
      dtostrf(t, 1, 2, TempString);
      Serial.print("Temperatura: ");
      Serial.println(TempString);
      publicarMsg("temperatura", TempString);
      
      if(t > tempIdeal){
        publicarMsg("switchAr", "1");
      }

      if(countLoopTemp >= 10 && t < (tempIdeal - 2)){
        publicarMsg("switchAr", "0");
        countLoopTemp = 0;
      }

      countLoopTemp++;
    }

    if(!isnan(h)){
      char HUMString[8];
      dtostrf(h, 1, 2, HUMString);
      Serial.print("Umidade: ");
      Serial.println(HUMString);
      publicarMsg("umidade", HUMString);

      if(h < umidIdeal){
        publicarMsg("switchUmid", "1");
      }

      if(countLoopUmid >= 10 && h > (umidIdeal - 2)){
        publicarMsg("switchUmid", "0");
        countLoopUmid = 0;
      }

      countLoopUmid++;
    }    
  }
}

void publicarMsg(char *topico, char *msg){

  client.publish(topico, msg);
  Serial.print("msg publicada: ");
  Serial.println(msg);
  piscar(2);
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