#// Normand Labossiere VE2VAX / VA2NQ SEPT-2018
#include <CayenneMQTTESP8266.h>
#define CAYENNE_DEBUG
#define CAYENNE_PRINT serial

#include <Wire.h>

#include <Adafruit_BME280.h> //sensor library Adafruit or other


char ssid[] = "BELLxxx";
char wifiPassword[] = "passw0rd";

char username[] = "xxxxxxxxx-xxxxxxxxxx-xxxxxxxxxxx";
char password[] = "xxxxxxxxx-xxxxxxxx-xxxxxxxxx";
char clientID[] = "xxxxxxxxxx-xxxxxxxxxxx-xxxxxxxxx";

// Virtual Pins of the BME280 widget.
#define TEMPERATURE_PIN V0
#define BAROMETER_PIN V1
#define HUMIDITY_PIN V2

Adafruit_BME280 bme; //Create an instance of the sensor


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
  bme.begin();
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
CAYENNE_OUT(V0)
{
  // Send the Temperature value to Cayenne
  Cayenne.virtualWrite(V0, bme.readTemperature());
}

CAYENNE_OUT(V1)
{
  // Send the Pressure value to Cayenne
  Cayenne.virtualWrite(V1, bme.readPressure());
}

CAYENNE_OUT(V2)
{
  // Send the Humidity value to Cayenne
  Cayenne.virtualWrite(V2, bme.readHumidity());
}
