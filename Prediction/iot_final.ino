#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <WiFi.h>
#include<DHT.h>
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME  "jaiadityaghorpade1715"
#define AIO_KEY  "aio_pLFJ62SB4wlNJYJOU5L3i6aceIIj"
#define dhtpin 4
#define dhttype DHT11
DHT dht(dhtpin,dhttype);
WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Subscribe pump = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/pump");
boolean MQTT_connect();

boolean MQTT_connect() {  int8_t ret; if (mqtt.connected()) {    return true; }  uint8_t retries = 3;  while ((ret = mqtt.connect()) != 0) { mqtt.disconnect(); delay(2000);  retries--;if (retries == 0) { return false; }} return true;}

Adafruit_MQTT_Publish humidity = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humidity");
Adafruit_MQTT_Publish moisture = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/moisture");
Adafruit_MQTT_Publish ph = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/ph");
Adafruit_MQTT_Publish temp = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temp");
//Adafruit_MQTT_Publish N = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/N");
//Adafruit_MQTT_Publish P = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/P");
//Adafruit_MQTT_Publish K = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/K");
Adafruit_MQTT_Publish textt = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/text");
int pumpp=5;
#define SensorPin 34
float calibration_value = 120.24 + 0.7;
unsigned long int avgValue; 
int buf[10];
int tempp;
float phValue;
void setup()
{
Serial.begin(9600);
  dht.begin();
  WiFi.disconnect();
  delay(3000);
  pinMode(36,INPUT);
  pinMode(pumpp,OUTPUT);
  digitalWrite(pumpp,HIGH);
  pinMode(SensorPin, INPUT);
  Serial.println("START");
  WiFi.begin("Home","123456789");

  mqtt.subscribe(&pump);
  while ((!(WiFi.status() == WL_CONNECTED))){
    delay(300);
    Serial.print("..");

  }
  Serial.println("Connected");

}


void loop()
{
  phh();
/*int n1 = random(10, 20);
int n2 = random(10, 20);
int n3 = random(10, 20);*/
int t = dht.readTemperature(); 
int h = dht.readHumidity(); 
int m=analogRead(36);
int mf=m/4;
Serial.println(mf);
//delay(1000);
    if(mf<800)
    {
      textt.publish("Please Motor stop");
      digitalWrite(pumpp,HIGH);
    }
    if(mf>850)
    {
      textt.publish("Plant Need Water !!!");
      digitalWrite(pumpp,LOW);
    }
    if (MQTT_connect()) {
    humidity.publish(h);
    moisture.publish(mf);
    ph.publish(phValue);
    temp.publish(t);
    //N.publish(n1);
    //P.publish(n2);
    //K.publish(n3); 
    delay(15000);
     Adafruit_MQTT_Subscribe *subscription_name;
      while ((subscription_name = mqtt.readSubscription(5000))) {
        if (subscription_name == &pump) {
          String myString = String(((char *)pump.lastread));
          Serial.println(myString);
          if(myString=="OFF")
          {
            digitalWrite(pumpp,HIGH);
          }
           if(myString=="ON")
          {
            digitalWrite(pumpp,LOW);
          }
          //Serial.println(((char *)pump.lastread));
        }

      }
    }    

}

void phh()
{
  
 for(int i=0;i<10;i++){ 
    buf[i]=analogRead(SensorPin);
    delay(10);
  }
  for(int i=0;i<9;i++){ 
    for(int j=i+1;j<10;j++){
      if(buf[i]>buf[j]){
        tempp=buf[i];
        buf[i]=buf[j];
        buf[j]=tempp;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)avgValue+=buf[i];  
    
  phValue=(float)avgValue*5.0/1024/6;
  phValue = -5.70 * phValue + calibration_value; 
  Serial.println(phValue);

}
