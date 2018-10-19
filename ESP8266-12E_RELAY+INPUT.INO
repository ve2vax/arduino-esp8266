#// Normand Labossiere VE2VAX / VA2NQ SEPT-2018
#include <CayenneMQTTESP8266.h>
#define CAYENNE_DEBUG
#define CAYENNE_PRINT serial


#define input 5       //gpio5= INPUT SWITCH opto-coupleur
#define relay 4      // gpio4 = relay

char ssid[] = "ve2vax";
char wifiPassword[] = "12345678";

char username[] = "3c89a750-4d65-11e8-a653-55835b4089d1";
char password[] = "592cf143807aa9080c4f59fa3c14543db826583c";
char clientID[] = "14dd2d30-c90c-11e8-8a08-61f9e5c4f140";




//Création des objets


void setup() 
{
  // put your setup code here, to run once:
  Cayenne.begin(username, password, clientID, ssid, wifiPassword);
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);  //LED bleu
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  pinMode(input, INPUT_PULLUP);  // pin input 
 
 // ne pas utiliser pin4 , pin 5  , 
 // elles sont utiliser pour I2C si c' est le cas.
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



CAYENNE_OUT(4)  //input  opto coupleur
{
  int val = digitalRead(input);                      // read the input pin 
  Cayenne.virtualWrite(input , val, TYPE_DIGITAL_SENSOR, UNIT_DIGITAL);
}
