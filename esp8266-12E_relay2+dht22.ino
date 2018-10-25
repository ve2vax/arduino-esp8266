// Normand Labossiere VE2VAX / VA2NQ OCT-2018
#include <CayenneMQTTESP8266.h>
#define CAYENNE_DEBUG
#define CAYENNE_PRINT serial


#include <DHT.h>          // Librairie des capteurs DHT

#define input 5       //gpio5= INPUT SWITCH opto-coupleur
#define relay 4      // gpio4 = relay

char ssid[] = "ve2vax";
char wifiPassword[] = "12345678";

char username[] = "xxxxxx-xxxxxx-xxxxxx";
char password[] = "xxxxxx-xxxxxxx-xxxxx";
char clientID[] = "xxxxxx-xxxx-xxxxxxxx";


#define DHTPIN 2    // Pin gpio2 le led sur lequel est branché le DHT
// Dé-commentez la ligne qui correspond à votre capteur 
//#define DHTTYPE DHT11       // DHT 11 
#define DHTTYPE DHT22         // DHT 22  (AM2302)   Besoin d`installer  pour la sonde  DHT22 , librairie  adafruite_sensor

//Création des objets
DHT dht(DHTPIN, DHTTYPE); 
unsigned long h = 0;
float t = 0;
void setup() 
{
  // put your setup code here, to run once:
  Cayenne.begin(username, password, clientID, ssid, wifiPassword);
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  pinMode(input, INPUT_PULLUP);  // pin input 
 
 // ne pas utiliser pin4 , pin 5  , 
 // elles sont utiliser pour I2C
 // pin2 = led bleu ,running ok, connected to wifi
  dht.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  Cayenne.loop();

}


CAYENNE_IN(7)  // not used   optionnal output.
{
  int currentValue = getValue.asInt();
  if (currentValue == 1)
  {
    //do whatever you want when you turn on the button on cayenne dashboard
    digitalWrite(relay, HIGH );
  }
  else
  {
    //do whatever you want when you turn off the button on cayenne dashboard
    digitalWrite(relay, LOW);
  }
}
CAYENNE_IN(8)  //Relay
{
  int currentValue = getValue.asInt();
  if (currentValue == 1)
  {
    //do whatever you want when you turn on the button on cayenne dashboard
    digitalWrite(relay, HIGH);
  }
  else
  {
    //do whatever you want when you turn off the button on cayenne dashboard
    digitalWrite(relay, LOW);
  }
}
// These functions are called when the Cayenne widget requests data for the Virtual Pin.


CAYENNE_OUT(V1)
{
  //Lecture de l'humidité ambiante
     h = dht.readHumidity();
    // Lecture de la température en Celcius
     t = dht.readTemperature();
    Cayenne.virtualWrite(V1, (t), TYPE_TEMPERATURE, UNIT_CELSIUS);
}

CAYENNE_OUT(V2)
{
  // Send the Humidity value to Cayenne
    h = dht.readHumidity();
    // Lecture de la température en Celcius
    t = dht.readTemperature();
    Cayenne.virtualWrite(V2, (h), TYPE_RELATIVE_HUMIDITY, UNIT_PERCENT);
}
CAYENNE_OUT(4)  //input  opto coupleur
{
  int val = digitalRead(input);                      // read the input pin 
  Cayenne.virtualWrite(V4 , val, TYPE_DIGITAL_SENSOR, UNIT_DIGITAL);
}
