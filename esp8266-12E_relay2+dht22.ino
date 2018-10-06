#// Normand Labossiere VE2VAX / VA2NQ SEPT-2018
#include <CayenneMQTTESP8266.h>
#define CAYENNE_DEBUG
#define CAYENNE_PRINT serial


#include <DHT.h>          // Librairie des capteurs DHT



char ssid[] = "BELL515";
char wifiPassword[] = "A441244D";

char username[] = "3c89a750-4d65-11e8-a653-55835b4089d1";
char password[] = "592cf143807aa9080c4f59fa3c14543db826583c";
char clientID[] = "14dd2d30-c90c-11e8-8a08-61f9e5c4f140";

// Virtual Pins of the BME280 widget.
#define TEMPERATURE_PIN V0
#define BAROMETER_PIN V1
#define HUMIDITY_PIN V2
#define DHTPIN 4    // Pin sur lequel est branché le DHT
// Dé-commentez la ligne qui correspond à votre capteur 
//#define DHTTYPE DHT11       // DHT 11 
#define DHTTYPE DHT22         // DHT 22  (AM2302)

//Création des objets
DHT dht(DHTPIN, DHTTYPE); 
unsigned long h = 0;
unsigned long t = 0;
void setup() 
{
  // put your setup code here, to run once:
  Cayenne.begin(username, password, clientID, ssid, wifiPassword);
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  pinMode(0, OUTPUT);
  digitalWrite(0, HIGH);
  pinMode(1, OUTPUT);
  digitalWrite(1, HIGH);
 // ne pas utiliser pin4 , pin 5  , 
 // elles sont utiliser pour I2C
 // pin2 = led bleu ,running ok, connected to wifi
  dht.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  Cayenne.loop();

}
CAYENNE_IN(5)
{
  int currentValue = getValue.asInt();
  if (currentValue == 1)
  {
    //do whatever you want when you turn on the button on cayenne dashboard
    digitalWrite(0, HIGH);
  }
  else
  {
    //do whatever you want when you turn off the button on cayenne dashboard
    digitalWrite(0, LOW);
  }
}
CAYENNE_IN(6)
{
  int currentValue = getValue.asInt();
  if (currentValue == 1)
  {
    //do whatever you want when you turn on the button on cayenne dashboard
    digitalWrite(1, HIGH);
  }
  else
  {
    //do whatever you want when you turn off the button on cayenne dashboard
    digitalWrite(1, LOW);
  }
}
CAYENNE_IN(7)
{
  int currentValue = getValue.asInt();
  if (currentValue == 1)
  {
    //do whatever you want when you turn on the button on cayenne dashboard
    digitalWrite(5, HIGH);
  }
  else
  {
    //do whatever you want when you turn off the button on cayenne dashboard
    digitalWrite(5, LOW);
  }
}
CAYENNE_IN(8)
{
  int currentValue = getValue.asInt();
  if (currentValue == 1)
  {
    //do whatever you want when you turn on the button on cayenne dashboard
    digitalWrite(0, LOW);
  }
  else
  {
    //do whatever you want when you turn off the button on cayenne dashboard
    digitalWrite(0, HIGH);
  }
}
// These functions are called when the Cayenne widget requests data for the Virtual Pin.


CAYENNE_OUT(V1)
{
  //Lecture de l'humidité ambiante
     h = dht.readHumidity();
    // Lecture de la température en Celcius
     t = dht.readTemperature();
    Cayenne.virtualWrite(V1, (t));
}

CAYENNE_OUT(V2)
{
  // Send the Humidity value to Cayenne
    Cayenne.virtualWrite(V2, (h));
}
