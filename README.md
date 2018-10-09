# arduino-esp8266
Example de projet, perso.   IOT  via myDevices 

Utilisant cayenne : IOT platform.

esp8266-12E_relay2+dht22.ino	 Example de ESP8266 generic avec  sonde temperature DHT22, et un relai

esp8266-12E_relay2.ino         Example de ESP8266 generic  avec un relai

esp8266_BME280.ino             Example de ESP8266 generic  avec une sonde BME280 ( I2C )
And more....  & 73
REF: pin`s see below

#define PIN_WIRE_SDA (4)
#define PIN_WIRE_SCL (5)

static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;

#define LED_BUILTIN 2

static const uint8_t D0   = 16;
static const uint8_t D1   = 5;
static const uint8_t D2   = 4;
static const uint8_t D3   = 0;
static const uint8_t D4   = 2;
static const uint8_t D5   = 14;
static const uint8_t D6   = 12;
static const uint8_t D7   = 13;
static const uint8_t D8   = 15;
static const uint8_t RX   = 3;
static const uint8_t TX   = 1;
