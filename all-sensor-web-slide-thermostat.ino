// Normand Labossiere VE2VAX / VA2NQ DeC-2018    Version 1.4.9.1 Incluant 
// Le  Projets  le plus complet  avec vsonde  dht22 et ds18b20 et les sondes bmp280,bme280
// Ce programme utilise le EEPROM pour eviter de reprogrammer le ESP8266 pour chaque projet Cayenne
// Il demarre en mode wifi access-point initialement pour sa configuration, avec l'adresse IP: 192.168.4.1
// et ainsi le programmer en serie et les distribuer,aux amis, et pour simplifier sa reutilisation.
// Et il sauvegarde  sa config dans  la memoire EEPROM
// Avec  les sondes DHT22,dht11 DS18b20 sont  connecter sur GPIO2 avec OneWire.
// Et entrée logic sur pin gpio5 sauf avec bmp280 ou bme280 ** voir sauf si .
// et relai sur pin gpio4 Et ajoute de custom widget  slide  comme thermostat programmable : utilisez channel 9 pour chaufage , pour  la climatisation
// le relai sera sur pin gpio3 , si la sonde est de type BME280 ou BMP280 ( Necesite une modification du circuit sauf si vous changer le fichier
//   pins_arduino.h , en changant les pins definition " SDA & SDC  de votre board/ hardware/esp8266
// et entrée logic sur pin gpio1  avec bmp280 ou bme280 ***NOTE IMPORTANT il faut modifier le circuit 
// ## ajout d'un watchdog timer .
#include <CayenneMQTTESP8266.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <DNSServer.h>
#define CAYENNE_DEBUG
#define CAYENNE_PRINT serial
// Select the define to Include all  you need for your sensor
//------------------------------------------------------------------------------
// Dé-commentez la ligne qui correspond à votre capteur  Librairie:OneWire version=2.3.4 author=Jim Studt, Tom Pollard, Robin James, Glenn Trewitt
// Pour les sondes de type DHT ou DS18B20 , onewire connection sur pin gpio2
//#define DHTTYPE DHT11     // DHT11 Librairie:DHT sensor library version=1.3.0 author=Adafruit
#define DHTTYPE DHT22     // DHT22 = (AM2302) Librairie:DHT sensor library version=1.3.0 author=Adafruit
//#define DS18B20 1         // pour sonde DS18B20 , selectionnez librairie:DallasTemperature version=3.8.0 author=Miles Burton
//#define BME280 1          // pour sonde bme280 , selectionnez Librairie:Adafruit BME280 Library version=1.0.7 author=Adafruit
//#define BMP280 1          // pour sonde bmp280 , selectionnez Librairie:Adafruit BMP280 Library version=1.0.2 author=Adafruit
//#define OLED_lcd
//#define BMP280_ADDRESS                (0x77)  //Address par defaut
//#define BME280_ADDRESS                (0x76)  //Address par defaut

//------------------------------------------------------------------------------

char* ssid_ap = "ve2ums-iot-20" ;  //Change the ssid for every devices you program
//                              //to prevent duplicated SSID in AP
#define delta_t 0.2     // delta hysteresis de temperature de thermostat 
#define bme_temp_offset -2.5    // valeur de correction pour sonde bme280 , il y a un ecart avec la lecture
#define input 5        // gpio5= INPUT SWITCH opto-coupleur  Sauf BME280 ou bmp280  voir #ifdef bmp280 ou bme280
#define relay 4        // gpio4 = relay   Sauf BME280 ou bmp280  voir #ifdef bmp280 ou bme280
#define relay_state 15 // Option voir fonction CAYENNE_OUT(V4) , pin  gpio15 connecter sur GPIO4,relay state read 
#ifdef OLED_lcd
 #include "Wire.h"
 #include <Adafruit_GFX.h>
 #include <Adafruit_SSD1306.h> 
 #define SCREEN_WIDTH 128 // OLED display width, in pixels
 #define SCREEN_HEIGHT 32 // OLED display height, in pixels
 // pins(14,12); //first # as SDA and second # as SCL
 // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
 #define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
 Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#endif

//
const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
int opto_in;
int softap = 0;
String esid = "";
String epass = "";
String index_esid;
String etatRelay = "";
String RelayValue = "";
String cayenne_passwd;
String cayenne_userid;
String cayenne_client_id;
String thermostat_type;
String on_red = "<span style='background-color: #FF0000'>ON</span>";
// objets
int temp_set;   //temperature thermostat valeur de depart par defaut on power on
String qctemp_set = "";

#ifdef BME280
 #include <Adafruit_BME280.h> //sensor library Adafruit or other
 //#define input 1        
 //#define relay 3
 Adafruit_BME280 bme; //Create an instance of the sensor
#endif

#ifdef BMP280
 //#define input 1        
 //#define relay 3
 #include <Adafruit_BMP280.h> //sensor library Adafruit or other
 Adafruit_BMP280 bme; //Create an instance of the sensor
#endif

#ifdef DHTTYPE
 #include <DHT.h>          // Librairie des capteurs DHT   + Adafruit unified sensor
 #define DHTPIN 2          // Pin gpio2 le led sur lequel est branché le DHT
 DHT dht(DHTPIN, DHTTYPE);
#endif
#ifdef DS18B20                // Data wire is plugged into port gpi0 2(LED) on the ESP8266
  #include <DallasTemperature.h>
  #include <DS18B20.h>
  #include <OneWire.h>
  #define ONE_WIRE_BUS 2       // Pin gpio2 le led sur lequel est branché la sonde DS18b20
  OneWire oneWire(ONE_WIRE_BUS);
  // Pass our oneWire reference to Dallas Temperature. 
  DallasTemperature ds18b20(&oneWire);
#endif

int h =0;
//unsigned long h = 0;
float t = 0;
float p = 0;
ESP8266WebServer server(80);

String st;
String content;
int statusCode;


// Section declaration de la string  de la page web "/test"
String getPage() {
  String page = "<html><meta charset='ISO-8859-1'><head><meta http-equiv='refresh' content='15'/test>";
  page += "<title>ESP8266-test</title>";
  page += "<meta http-equiv='Content-Type' content='text/html;charset=utf-8' /><style> body { background-color: #fffff; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }</style>";
  page += "</head><body><h2>ESP8266 Test</h2>";
  page += "<h3>DHT22</h3>";
  page += "<ul><li>Temp&#233;rature : ";
  page += t;
  page += "&#176;C</li>";
  page += "<li>Humidit&#233; : ";
  page += h;
  page += "%</li>";
  page += "<li>Entr&#233;e 0/1: ";
  page += opto_in;
  page += "</li></ul>";
  page += "<ul><li>Temp&#233;rature set: ";
  page += temp_set;
  page += "</li></ul><br>";
  page += "<h3>Relai</h3>";
  page += "<form action='/test' method='POST'>";
  page += "<ul><li>Relai (&#233;tat: ";
  page += etatRelay;
  page += ")";
  page += "<h3><INPUT type='radio' name='Relai_b' value='1'>ON";
  page += "<INPUT type='radio' name='Relai_b' value='0'>OFF</li></ul>";
  page += "<br><input type='radio' name='thermostat_type' value='0'> <label> cooling: </label>";
  page += "<input type='radio' name='thermostat_type' value='1'> <label> heating: </label>";
  page += "<input type='radio' name='thermostat_type' value='2'> <label> disable: </label>"; // checked='checked'
  page += "<INPUT type='submit' value='Actualiser'></h3>";
  page += "<br>";
  page += "</body></html>";
  return page;
}
void get_temp() {
  #ifdef BMP280
   t = (bme.readTemperature()) + bme_temp_offset;
  #endif
  #ifdef BME280
   t = (bme.readTemperature()) + bme_temp_offset;
   h = bme.readHumidity();
  #endif
  #ifdef DHTTYPE 
   t = dht.readTemperature();
   h = dht.readHumidity();
  #endif
  #ifdef DS18B20 
   ds18b20.requestTemperatures(); // Send the command to get temperatures for DS18b20
   delay(100);
   t = ds18b20.getTempCByIndex(0);
  #endif 
}
void relai_on() {
  digitalWrite(relay, LOW);
  etatRelay = "Off";
  RelayValue == "0";
  
}
void relai_off() {
  digitalWrite(relay, HIGH);
  etatRelay = (on_red);
  RelayValue == "1";
}
void handle_cleareeprom() {
    content = "<!DOCTYPE HTML>\r\n<html>";
    content += "<p><h4>Clearing the EEPROM</h4></p></html>";
    server.send(200, "text/html", content);
    Serial.println("clearing eeprom");
    for (int i = 0; i < 300; ++i) {
      EEPROM.write(i, 0);
    }
    EEPROM.commit();
    reset_wifi();
    softap = 0;
    delay(100);
}
void handleTest() {
  if (server.hasArg("thermostat_type")) {
    thermostat_type = server.arg("thermostat_type");
    Serial.print("changed thermostat_type value:");
    Serial.println(thermostat_type);
  }
  if (server.hasArg("Relai_b")) {
    Serial.print("rcv new value Relai_b:");
    RelayValue = server.arg("Relai_b");
    if (RelayValue == "0") { relai_on(); }
    else { relai_off(); }
    Serial.print(RelayValue);
    handleSubmit();
  } else {
    opto_in = digitalRead(input);                      // read the input pin
    server.send (200, "text/html", getPage());
    get_temp();
    }
}


void handleSubmit() {
  // Change le GPIO relay / Update GPIO Relay from web /test page
  RelayValue = server.arg("Relai_b");
  Serial.println("Set relay ");
  Serial.print(RelayValue);
  if (RelayValue == "1") {
    relai_off();
    server.send (200, "text/html", getPage()); }
  else if (RelayValue == "0") {
      relai_on();
      server.send (200, "text/html", getPage()); }
      else {
      Serial.println("Error Relay Value"); }
}

void setup() {
  //ESP   Watchdog timer 
ESP.wdtDisable();
ESP.wdtEnable(WDTO_8S);

#ifdef OLED_lcd
 // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
 // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  display.begin(0, 0x3C, false);
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(0,0);
    display.println("Starting");
    display.display(); 
    delay(3000);
    display.clearDisplay();
#endif
#ifdef DHTTYPE
  dht.begin();
#endif  
#ifdef DS18B20
  ds18b20.begin(); //ds18b20  
#endif
#ifdef BME280
  bme.begin();
  opto_in = 0;
#endif
#ifdef BMP280
 bme.begin();
#endif#
  
  Serial.begin(115200);
  EEPROM.begin(300);
  delay(1000);
  reset_wifi();
  //Setup cayenne variable et env.
  pinMode(relay_state, INPUT);
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);
  pinMode(input, INPUT_PULLUP);  // pin input

  // ne pas utiliser pin4 , pin 5 si vous utilisez device I2c ,
  // elles sont utiliser pour I2C
  // pin2 = led bleu on or blinking ,if running ok, connected to wifi
  //
  Serial.println();
  Serial.println();
  Serial.println("Startup");
  // Lecture du eeprom pour wifi, ssid & pass
  Serial.println("Lecture du EEPROM ssid");
  for (int i = 0; i < 32; ++i)
  {
    esid += char(EEPROM.read(i));
  }
  Serial.print("SSID: ");
  Serial.println(esid);
  Serial.println("Lecture du  EEPROM wifi passwd");

  for (int i = 32; i < 96; ++i)
  {
    epass += char(EEPROM.read(i));
  }
  Serial.print("PASSWORD: ");
  Serial.println(epass);
  // les 3 variables pour Cayenne maintenant
  // Cayenne Lecture cayenne_uid+cayenne_passwd + client_id
  Serial.println("Lecture du EEPROM Cayenne userid");

  for (int i = 96; i < 160; ++i)
  {
    cayenne_userid += char(EEPROM.read(i));
  }
  Serial.print("Cayenne userid: ");
  Serial.println(cayenne_userid);
  //
  // Cayenne Lecture cayenne_userid+cayenne_passwd + cayenne_client_id
  Serial.println("Lecture du EEPROM Cayenne passwd");

  for (int i = 160; i < 224; ++i)
  {
    cayenne_passwd += char(EEPROM.read(i));
  }
  Serial.print("Cayenne passwd: ");
  Serial.println(cayenne_passwd);
  //
  // Cayenne Lecture cayenne_uid+cayenne_passwd + client_id
  Serial.println("Lecture du EEPROM cayenne_client_id");
  for (int i = 224; i < 288; ++i)
  {
    cayenne_client_id += char(EEPROM.read(i));
  }
  for (int i = 288; i < 292; ++i)
  {
    thermostat_type += char(EEPROM.read(i));
  }
  readeeprom_temp_set();
  //temp_set = (temp_set_str.toInt());
  Serial.print("temp_set: ");
  Serial.println(temp_set);
  //EEPROM.commit();
  // Lecture des variables dans  le eeprom terminer
  //
  if ( esid.length() > 1 ) {
  reset_wifi();
    Serial.print("trying wifi... ");
    WiFi.hostname(ssid_ap);
    WiFi.begin(esid.c_str(), epass.c_str());
    Serial.print("....ok ");
    if (testWifi()) {
      Serial.print("web start mode=0");
      softap = 0;
      launchWeb(0);
      return;
    }
  }
  setupAP();
}
void readeeprom_temp_set(void) { // write my Integer into eeprom 2 bytes used
  byte temp_set_high = (EEPROM.read(298));
  byte temp_set_low = (EEPROM.read(299));
  temp_set = word(temp_set_high, temp_set_low);
}  
bool testWifi(void) {
  int c = 0;
  Serial.println("Waiting for Wifi to connect");
  while ( c < 25 ) {
    if (WiFi.status() == WL_CONNECTED) {
      return true;
    }
    delay(300);
    Serial.print(WiFi.status());
    c++;
  }
  Serial.println("");
  Serial.println("Connect timed out, try AccessPoint");
  return false;
}

void launchWeb(int webtype) {
  Serial.println("");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  createWebServer(webtype);
  // Start the server
  Serial.println("Server started.");
  if (softap == 0) {
    delay(100);
    Cayenne.begin(cayenne_userid.c_str(), cayenne_passwd.c_str(), cayenne_client_id.c_str());   //, esid.c_str(), epass.c_str());
    Serial.println("Cayenne begin, Cayenne IOT ready .");
    Serial.println((WiFi.status()));
  }

  server.begin();
  Serial.println("HTTP Server started ... ");
  delay(500);
  Serial.println((WiFi.status()));
}

void setupAP(void) {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  softap = 0;
  delay(100);
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(200);
    }
  }
  Serial.println("");
  st = "<h5>-List of WiFi available:</h5><ol>";
  for (int i = 0; i < n; ++i)
  {
    // Print SSID and RSSI for each network found
    st += "<li>";
    st += WiFi.SSID(i);
    st += " (";
    st += WiFi.RSSI(i);
    st += ")";
    st += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*";
    st += "</li>";
  }
  st += "</ol>";
  delay(200);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid_ap);
  Serial.println("softap");
  softap = 1;
  dnsServer.start(DNS_PORT, "*", apIP);
  delay(1000);
  launchWeb(1);
  Serial.println("softAP Ready-over");

}
void reset_wifi() {
  WiFi.disconnect();
  WiFi.softAPdisconnect(false);  // Turn off  wifi Accesspoint
  WiFi.enableAP(false);
}
void createWebServer(int webtype)
{
  if ( webtype == 1 ) {
    server.on("/", []() {
      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      content += "<!DOCTYPE HTML>\r\n<html>";
      content += "<head><meta http-equiv='Content-Type' content='text/html; charset=utf-8' /><style> body { background-color: #fffff; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }</style>";
      content += "</head><title>Ve2vax IOT setup</title><body><h3><p align='center'>Allo VE2VAX from ESP8266-IOT at ";
      content += ipStr;
      content += "</p>";
      content += "<p align='center'>VE2VAX IOT SETUP</p></h3>";
      content += st;
      content += "<h4><form method='get' action='setting'><label>SSID:<br> </label><input name='ssid' length=32><br><label>WiFi password:<br>  </label><input name='pass' length=32><br><label>Cayenne_userid:<br> </label><input name='cayenne_userid' length=64><br><label>Cayenne_password:<br> </label><input name='cayenne_passwd' length=64><br><label>Cayenne_client_id:<br> </label><input name='cayenne_client_id' length=64> ";
      content += "<br><input type='radio' name='thermostat_type' value='0'> <label> cooling: </label>";
      content += "<input type='radio' name='thermostat_type' value='1'> <label> heating: </label>";
      content += "<input type='radio' name='thermostat_type' value='2' checked='checked' /> <label> disable: </label>";
      content += "<input type='number' name='temp_set' min='0' max='30'> <LABEL> target temperature set</LABEL></li></lu>";
      content += "<br><input type='submit'></form></h4>";
      content += "<p>Click on the submit button when all boxes are filled, and the input will be stored in the eeprom of the ESP8266 .</p>";
      content += "<p>sub page available: /reset for reboot, /cleareeprom ,/ip , /test</p>";
      content += "<br><form><input type='button' value='Controle IOT' onclick=""location.href='../test'""></form>";
      content += "<p>&#169; VE2VAX </p></body></html>";
      server.send(200, "text/html", content);
    });
    server.on( "/test", handleTest );

    server.on ( "/cleareeprom", handle_cleareeprom );
    
    server.onNotFound([]() {
    String message = "<!DOCTYPE HTML>\r\n<html>";
    message += "<h4>Hello IOT Setup</h4>\n\n";
    message += "<head><meta http-equiv='Refresh' content='5; url=http://192.168.4.1/'></head></html>";
    server.send(200, "text/html", message);
    });
    
    server.on("/setting", []() {
      String qsid = server.arg("ssid");
      String qpass = server.arg("pass");
      String qcuid = server.arg("cayenne_userid");
      String qcpass = server.arg("cayenne_passwd");
      String qcclid = server.arg("cayenne_client_id");
      String qcthermostat_type = server.arg("thermostat_type");
      String qctemp_set = server.arg("temp_set");
      temp_set = qctemp_set.toInt();
      
      if (qsid.length() > 0 && qpass.length() > 0) {
        Serial.println("clearing eeprom");
        for (int i = 0; i < 300; ++i) {
          EEPROM.write(i, 0);
        }
        Serial.println(qsid);
        Serial.println("");
        Serial.println(qpass);
        Serial.println("");
        Serial.println(qcuid);
        Serial.println("");
        Serial.println(qcpass);
        Serial.println("");
        Serial.println(qcclid);
        Serial.println("");
        Serial.println(qcthermostat_type);
        Serial.println("");
        Serial.println(temp_set);
        Serial.println("");
        Serial.println("writing eeprom ssid:");
        for (int i = 0; i < qsid.length(); ++i)
        {
          EEPROM.write(i, qsid[i]);
          Serial.print("Wrote: ");
          Serial.println(qsid[i]);
        }
        Serial.println("writing eeprom pass:");
        for (int i = 0; i < qpass.length(); ++i)
        {
          EEPROM.write(32 + i, qpass[i]);
          Serial.print("Wrote: ");
          Serial.println(qpass[i]);
        }
        Serial.println("writing eeprom cayenne_userid:");
        for (int i = 0; i < qcuid.length(); ++i)
        {
          EEPROM.write(96 + i, qcuid[i]);
          Serial.print("Wrote: ");
          Serial.println(qcuid[i]);
        }
        Serial.println("writing eeprom cayenne_passwd:");
        for (int i = 0; i < qcpass.length(); ++i)
        {
          EEPROM.write(160 + i, qcpass[i]);
          Serial.print("Wrote: ");
          Serial.println(qcpass[i]);
        }
        Serial.println("writing eeprom cayenne_client_id:");
        for (int i = 0; i < qcclid.length(); ++i)
        {
          EEPROM.write(224 + i, qcclid[i]);
          Serial.print("Wrote: ");
          Serial.println(qcclid[i]);
        }
        for (int i = 0; i < qcthermostat_type.length(); ++i)
        {
          EEPROM.write(288 + i, qcthermostat_type[i]);
          Serial.print("Wrote: ");
          Serial.println(qcclid[i]);
        }
        EEPROM.write(298, highByte(temp_set));
        EEPROM.write(299, lowByte(temp_set));
        Serial.print("Wrote: ");
        Serial.println(temp_set);
        EEPROM.commit();
        content = "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}";
                                  statusCode = 200;
      } else {
        content = "{\"Error\":\"404 not found\"}";
        statusCode = 404;
        Serial.println("Sending 404");
      }
      server.send(statusCode, "application/json", content);
    });
  }
  else if (webtype == 0) {
    server.on("/ip", []() {
      IPAddress ip = WiFi.localIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      server.send(200, "application/json", "{\"IP\":\"" + ipStr + "\"}");
    });
    server.on( "/", handleTest );

    server.on( "/test", handleTest );

    server.on ( "/cleareeprom", handle_cleareeprom );
    
    server.on("/reset", []() {
      content = "<!DOCTYPE HTML>\r\n<html>";
      content += "<p>Rebooting ESP8266-IOT</p></html>";
      server.send(200, "text/html", content);
      Serial.println("....");
      delay(500);
      reset_wifi();
      ESP.restart();
    });
  }
}

void thermostat_heat() {
  get_temp();
  if ((temp_set >= 1 ) && ( t > temp_set )) // si la temperature est= ou sous ZERO, coupe le chauffage
  { // ou allume le chauffage si  temp_set value  < t
    //turn off the heating
    relai_on();
  }
  else
  {
    //turn on the heating
    if ((temp_set >= 1 ) && ( (t+delta_t) < temp_set )) {
    relai_off(); }
  }
}

void thermostat_cool() {
  get_temp();
  if ((temp_set >= 1) && (t < temp_set)) {  // si la temperature est= ou sous ZERO, coupe la climatisation
    // ou allume lea climatisation si  temp_set value  < t
    // turn off the cooling
    relai_on(); }
  else
  {
    //turn on the cooling
    if ((temp_set >= 1) && ((t-delta_t) > temp_set)) { relai_off(); } 
  }
}
void thermostat_event() {
  if (thermostat_type == "1") {
     thermostat_heat();
  }  // Thermostat pour chauffage
  if (thermostat_type == "0") {
     thermostat_cool();
    }
}          
void loop() // boucle  principale
{ 
  ESP.wdtFeed();
  if (softap == 1) { dnsServer.processNextRequest(); }
  if (WiFi.status() == WL_CONNECTED) {
    Cayenne.loop();
    thermostat_event();
  server.handleClient(); }
  else 
  {
    WiFi.begin(esid.c_str(), epass.c_str());
    //Cayenne.begin(cayenne_userid.c_str(), cayenne_passwd.c_str(), cayenne_client_id.c_str()); 
  }
  server.handleClient();
  thermostat_event();
}
// Les fonctions de cayenne called by  Cayenne.loop
CAYENNE_IN(8) {
  int currentValue = getValue.asInt();
  if (currentValue == 1)
  {
    //do whatever you want when you turn on the button on cayenne dashboard
    relai_off();
  }
  else {
    
  }
  {
    //do whatever you want when you turn off the button on cayenne dashboard
    relai_on();
  }
}
CAYENNE_OUT(V8)
{
  int value = digitalRead(relay);  // read the input pin
  if (value == 1) {
    Cayenne.virtualWrite(V8 , 1, TYPE_DIGITAL_SENSOR, UNIT_DIGITAL);
    relai_off();
  }
  else {
    Cayenne.virtualWrite(V8 , 0, TYPE_DIGITAL_SENSOR, UNIT_DIGITAL);
    relai_on(); 
  }

}
// Fonction Cayenne  pour lire la valeur du thermostat desirer via cayenne avec le custom widget slider
CAYENNE_IN(V9)  // channel 9
{
  int t_temp_set = getValue.asInt();
  get_temp();
  Serial.println("readed temp_set from cayenne");
  Serial.print(t_temp_set);
  Cayenne.virtualWrite(V9, t_temp_set);
  if ((temp_set) != (t_temp_set)) {
    temp_set = t_temp_set;
    
    EEPROM.write(298, highByte(t_temp_set));
    EEPROM.write(299, lowByte(t_temp_set));
    EEPROM.commit();
    Serial.print("EEPROM Wrote: ");
    Serial.println(temp_set); }
    Cayenne.virtualWrite(V9, temp_set);
}

CAYENNE_OUT(V1)
  {
    // Lecture de la température en Celcius
    char str_t[5];     //result string 5 positions + \0 at the end
    get_temp();
    // converti float to string type
    // format 5 positions with 2 decimal places
    //
    dtostrf(t, 4, 2, str_t );
    #ifdef OLED_lcd
     
     display.setTextSize(2);
     display.setCursor(0,0);
     display.clearDisplay();
     display.print(t);
     display.print(" 'C");
     display.display();
    #endif
    //
    Serial.println(thermostat_type);
    Cayenne.virtualWrite(V1, (str_t), TYPE_TEMPERATURE, UNIT_CELSIUS);
    if (thermostat_type == "1") {
      thermostat_heat();
    }  // Thermostat pour chauffage
    if (thermostat_type == "0") {
      thermostat_cool();
    }  // Thermostat pour climatisation
  }
#ifdef DHTTYPE
  CAYENNE_OUT(V2)
  {
    //. Lecture  Humidite
    // Send the Humidity value to Cayenne
    h = dht.readHumidity();
    #ifdef OLED_lcd
     display.setTextSize(2);
     display.setCursor(0,16);
     display.print(h);
     display.print(" %");
     display.display();
    #endif
    Cayenne.virtualWrite(V2, (h), TYPE_RELATIVE_HUMIDITY, UNIT_PERCENT);
  }
#endif  
#ifdef BME280
  CAYENNE_OUT(V2)
  {
    //. Lecture  Humidite
    // Send the Humidity value to Cayenne
    h = bme.readHumidity();
    #ifdef OLED_lcd
     display.setTextSize(2);
     display.setCursor(0,16);
     display.print(h);
     display.print(" %");
     display.display();
    #endif
    Cayenne.virtualWrite(V2, (h), TYPE_RELATIVE_HUMIDITY, UNIT_PERCENT);
  }
 #endif
#ifdef BMP280 
  CAYENNE_OUT(V0)
  {
    //. Lecture  pression Atmospherique
    // Send the presure value to Cayenne
    p = bme.readPressure();
    #ifdef OLED_lcd
     
     display.setTextSize(2);
     display.setCursor(0,16);
     display.print(h);
     display.print(" %");
     display.display();
    #endif
    Cayenne.virtualWrite(V0, (p), TYPE_BAROMETRIC_PRESSURE, UNIT_HECTOPASCAL);
  }
#endif
#ifdef BME280 
  CAYENNE_OUT(V0)
  {
    //. Lecture  pression Atmospherique
    // Send the presure value to Cayenne
    p = (30 - (bme.readPressure() * 0.0002953));
   
    #define UNIT_INHG "Hg" // pouce Mercure
    Cayenne.virtualWrite(V0, (p)); //, TYPE_BAROMETRIC_PRESSURE, UNIT_INHG); //UNIT_HECTOPASCAL
  }
#endif  
  CAYENNE_OUT(V3)  //input  opto coupleur
  {
    opto_in = digitalRead(input);                      // read the input pin
    Cayenne.virtualWrite(V3 , opto_in, TYPE_DIGITAL_SENSOR, UNIT_DIGITAL);
  }
  CAYENNE_OUT(V9) {      // custom widget slide to set thermostat value > Cayenne
    Cayenne.virtualWrite(V9, temp_set);
    Serial.println(temp_set);
  }
  //CAYENNE_OUT(V4)
  //  Cayenne.virtualWrite(V4 , opto_in, TYPE_DIGITAL_SENSOR, UNIT_DIGITAL);
  //}
  // Fonction Cayenne optionnel pour lire l'etat du relay si vous avez brancher la sortie sur (relay_state)
  //CAYENNE_OUT(V4)
  //{
  //  int value = digitalRead(relay_state);                      // loop relay state read, read the input pin
  //  Cayenne.virtualWrite(V4 , value, TYPE_DIGITAL_SENSOR, UNIT_DIGITAL);
  //}
