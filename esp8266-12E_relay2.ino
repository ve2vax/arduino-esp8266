#include <CayenneMQTTESP8266.h>
#define CAYENNE_DEBUG
#define CAYENNE_PRINT serial

char ssid[] = "BELLxxx";
char wifiPassword[] = "passw0rd";

char username[] = "xxxxxxxxxxxxxxx-xxxxxxxxxxx-xxxxxxxxx";
char password[] = "xxxxxxxxx-xxxxxxxxxxx-xxxxxxxxxxxxxx";
char clientID[] = "xxxxxxx-xxxxxxxxx-xxxxxxxxxxxx";

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
