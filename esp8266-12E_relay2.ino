#include <CayenneMQTTESP8266.h>
#define CAYENNE_DEBUG
#define CAYENNE_PRINT serial

char ssid[] = "BELL515";
char wifiPassword[] = "A441244D";

char username[] = "3c89a750-4d65-11e8-a653-55835b4089d1";
char password[] = "592cf143807aa9080c4f59fa3c14543db826583c";
char clientID[] = "075ed260-c90e-11e8-9555-33faa5c5b5a8";

void setup() {
  // put your setup code here, to run once:
  Cayenne.begin(username, password, clientID, ssid, wifiPassword);
  //pinMode(2, OUTPUT);
  //digitalWrite(2, LOW);
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);

  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  Cayenne.loop();

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
    digitalWrite(4, HIGH);
  }
  else
  {
    //do whatever you want when you turn off the button on cayenne dashboard
    digitalWrite(4, LOW);
  }
}
