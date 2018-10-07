#include <CayenneMQTTESP8266.h>
#define CAYENNE_DEBUG
#define CAYENNE_PRINT serial     
#define DIGITAL_PIN 5       //INPUT SWITCH 
int inPin = 5;
int val = 0; 

char ssid[] = "ve2vax";
char wifiPassword[] = "ve2vaxva2nq";

char username[] = "XXXXxxxx-xxxxx";
char password[] = "xxxxx-xxxxx-xxxxx";
char clientID[] = "xxxx-xxxx-xxxxxxx";

void setup() {
  // put your setup code here, to run once:
  Cayenne.begin(username, password, clientID, ssid, wifiPassword);
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  pinMode(5, INPUT_PULLUP); // pin input 
  

  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  Cayenne.loop();

}

// These functions are called when the Cayenne widget requests data for the Virtual Pin.
CAYENNE_OUT(0)
{
  int val = digitalRead(inPin);                      // read the input pin 
  Cayenne.virtualWrite(DIGITAL_PIN , val);
}


CAYENNE_IN(6)
{
  int currentValue = getValue.asInt();
  if (currentValue == 1)
  {
    //do whatever you want when you turn on the button on cayenne dashboard
    digitalWrite(2, HIGH);
  }
  else
  {
    //do whatever you want when you turn off the button on cayenne dashboard
    digitalWrite(2, LOW);
  }
}

CAYENNE_IN(8)
{
  int currentValue = getValue.asInt();
  if (currentValue == 1)
  {
    //do whatever you want when you turn on the button on cayenne dashboard
    digitalWrite(4, HIGH);
  }
  else
  {
    //do whatever you want when you turn off the button on cayenne dashboard
    digitalWrite(4, LOW);
  }
}
