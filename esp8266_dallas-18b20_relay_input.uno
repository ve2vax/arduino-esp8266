// Normand Labossiere VE2VAX / VA2NQ nov-2018 A essayer BETA
#include <CayenneMQTTESP8266.h>
#define CAYENNE_DEBUG
#define CAYENNE_PRINT serial


// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port gpi0 2(LED) on the ESP8266
#define ONE_WIRE_BUS 2

#define input 5       //gpio5= INPUT SWITCH opto-coupleur
#define relay 4      // gpio4 = relay

char ssid[] = "ve2vax";
char wifiPassword[] = "12345678";

char username[] = "xxxxxx-xxxxxx-xxxxxx";
char password[] = "xxxxxx-xxxxxxx-xxxxx";
char clientID[] = "xxxxxx-xxxx-xxxxxxxx";



//Création des objets
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensor(&oneWire);


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
  sensor.begin();

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
    sensor.requestTemperatures(); // Send the command to get temperatures
   
     
    // Lecture de la température en Celcius
     
    Cayenne.virtualWrite(V1, (sensor.getTempCByIndex(0)), TYPE_TEMPERATURE, UNIT_CELSIUS);
}


CAYENNE_OUT(4)  //input  opto coupleur
{
  int val = digitalRead(input);                      // read the input pin 
  Cayenne.virtualWrite(V4 , val, TYPE_DIGITAL_SENSOR, UNIT_DIGITAL);
}
