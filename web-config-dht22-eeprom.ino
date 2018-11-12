// Normand Labossiere VE2VAX / VA2NQ Nov-2018
//
// Ce programme est  pour eviter de reprogrammer le ESP8266 pour chaque projet
// Il demarre en mode wifi access-point initialement pour le configurer avec l'adresse IP: 192.168.4.1
// et ainsi le programmer en serie et les distribuer,aux amis, et pour simplifier sa reutilisation.
// Et il sauvegarde  sa config dans  la memoire EEPROM
// Avec  sonde DHT22 connecter sur GPIO2 OneWire
// Et entrée loginc sur pin gpio5
// et relai sur pin gpio4
#include <CayenneMQTTESP8266.h>

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <time.h>
#define CAYENNE_DEBUG
#define CAYENNE_PRINT serial

#include <DHT.h>          // Librairie des capteurs DHT
#define input 5           // gpio5= INPUT SWITCH opto-coupleur
#define relay 4           // gpio4 = relay
#define relay_state 15    // Option voir fonction CAYENNE_OUT(V4) , pin  gpio15 connecter sur GPIO4,relay state read 
#define DHTPIN 2          // Pin gpio2 le led sur lequel est branché le DHT
// Dé-commentez la ligne qui correspond à votre capteur 
//#define DHTTYPE DHT11   // DHT11 
#define DHTTYPE DHT22     // DHT22 = (AM2302)

int val;
int softap = 0;
String esid = "";
String epass = "";
String index_esid;
String etatRelay = "Off";
String cayenne_passwd;
String cayenne_userid;
String cayenne_client_id;
// objets
DHT dht(DHTPIN, DHTTYPE); 
unsigned long h = 0;
float t = 0;

ESP8266WebServer server(80);

char* ssid = "ve2vax-iot-18";     //Change the ssid for every devices you programme , to prevent duplicated SSID in AP

String st;
String content;
int statusCode;

// Section declaration de la string  de la page web "/test"
String getPage(){
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
  page += val;
  page += "</li></ul><br>";
  page += "<h3>Relai</h3>";
  page += "<form action='/test' method='POST'>";
  page += "<ul><li>Relai (&#233;tat: ";
  page += etatRelay;
  page += ")";
  page += "<h3><INPUT type='radio' name='Relai_b' value='0'>ON";
  page += "<INPUT type='radio' name='Relai_b' value='1'>OFF</li></ul>";
  page += "<INPUT type='submit' value='Actualiser'></h3>";
  page += "<br>";
  page += "</body></html>";
  return page;
}

void handleTest(){ 
  if (server.hasArg("Relai_b")) {
    handleSubmit();
  } else {
    server.send (200, "text/html", getPage());
  t = dht.readTemperature();
  h = dht.readHumidity();
  val = digitalRead(input);                      // read the input pin 
  }
}

void handleSubmit() {
  // Change le GPIO relay / Update GPIO Relay from web /test page
  String RelayValue;
  RelayValue = server.arg("Relai_b");
  Serial.println("Set relay "); 
  Serial.print(RelayValue);
  if (RelayValue == "1") {
    digitalWrite(relay, LOW);
    etatRelay = "Off";
    server.send (200, "text/html", getPage());
  } else if (RelayValue == "0") {
    digitalWrite(relay, HIGH);
    etatRelay = "On";
    server.send (200, "text/html", getPage());
  } else {
    Serial.println("Error Relay Value");
  }
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  delay(1000);
  WiFi.softAPdisconnect(false);  // Turn off  wifi Accesspoint after 0ne hour
  WiFi.enableAP(false);
    // run premiere fois  pour initialiser le eeprom et remettre les // aux 3 lignes ici bas
      //Serial.println("clearing eeprom");
      //for (int i = 0; i < 300; ++i) { EEPROM.write(i, 0); }
      //EEPROM.commit();
  //
  //Setup cayenne variable et env.
      pinMode(relay_state, INPUT);
      pinMode(2, OUTPUT);
      digitalWrite(2, LOW);
      pinMode(4, OUTPUT);
      digitalWrite(4, LOW);
      pinMode(input, INPUT_PULLUP);  // pin input 
 
      // ne pas utiliser pin4 , pin 5 si vous utilisez device I2c , 
      // elles sont utiliser pour I2C
      // pin2 = led bleu on or blinking ,if running ok, connected to wifi
      dht.begin();
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
  Serial.print("Cayenne client_id: ");
  Serial.println(cayenne_client_id);
  // Lecture des variables dans  le eeprom terminer

  //
   if ( esid.length() > 1 ) {
      Serial.print("trying wifi... ");
      WiFi.hostname(ssid);
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

bool testWifi(void) {
  int c = 0;
  Serial.println("Waiting for Wifi to connect");  
  while ( c < 25 ) {
    if (WiFi.status() == WL_CONNECTED) {
      return true; } 
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
  Serial.println("WiFi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  createWebServer(webtype);
  // Start the server
  Serial.println("Server started."); 
  if (softap == 0) { 
    // WiFi.disconnect();
    delay(1000);
    Cayenne.begin(cayenne_userid.c_str(), cayenne_passwd.c_str(), cayenne_client_id.c_str());   //, esid.c_str(), epass.c_str());
    Serial.println("Cayenne begin, Cayenne IOT ready .");
    Serial.println((WiFi.status()));
  }
  
  server.begin(); 
  Serial.println("HTTP Server started ... ");  
  delay(2000);
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
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
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
      st += (WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*";
      st += "</li>";
    }
  st += "</ol>";
  delay(200);
  
  WiFi.softAP(ssid);
  Serial.println("softap");
  softap = 1;
  delay(1000);
  launchWeb(1);
  Serial.println("softAP Ready-over");
  
}

void createWebServer(int webtype)
{
  if ( webtype == 1 ) {
    server.on("/", []() {
        IPAddress ip = WiFi.softAPIP();
        String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
        content += "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Transitional//EN' 'xhtml1/xhtml1-transitional.dtd'>";
        content += "<head><meta http-equiv='Content-Type' content='text/html; charset=utf-8' /><style> body { background-color: #fffff; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }</style>";
        content += "<h3><p align='center'>Hello VE2VAX from ESP8266-IOT at ";
        content += ipStr;
        content += "</p>";
        content += "<p align='center'>VE2VAX IOT SETUP</p></h3>";
        content += st;
        content += "</p><h4><form method='get' action='setting'><label>SSID:<br> </label><input name='ssid' length=32><br><label>WiFi password:<br>  </label><input name='pass' length=32><br><label>Cayenne_userid:<br> </label><input name='cayenne_userid' length=64><br><label>Cayenne_password:<br> </label><input name='cayenne_passwd' length=64><br><label>client_id:<br> </label><input name='Cayenne_client_id' length=64><br>";
        content += "<br><input type='submit'></form></h4>";
        content += "<p>Click on the submit button when all boxes are filled, and the input will be stored in the eeprom of the ESP8266 .</p>";
        content += "<p>sub page available: /reset for reboot, /cleareeprom </p>";
        content += "<br><form><input type='button' value='Controle IOT' onclick="location.href='../test'"></form>";
        content += "<p> &#169; VE2VAX </p></html>";
        server.send(200, "text/html", content);  
   });
    server.on( "/test", handleTest );

    server.on("/cleareeprom", []() {
      content = "<!DOCTYPE HTML>\r\n<html>";
      content += "<p>Clearing the EEPROM</p></html>";
      server.send(200, "text/html", content);
      Serial.println("clearing eeprom");
      for (int i = 0; i < 288; ++i) { EEPROM.write(i, 0); }
      EEPROM.commit();
      WiFi.disconnect();
      WiFi.softAPdisconnect(false);  // Turn off  wifi Accesspoint after 0ne hour
      WiFi.enableAP(false);
      softap = 0;
      delay(500);
      ESP.restart();
    });
    server.on("/setting", []() {
        String qsid = server.arg("ssid");
        String qpass = server.arg("pass");
        String qcuid = server.arg("cayenne_userid");
        String qcpass = server.arg("cayenne_passwd");
        String qcclid = server.arg("cayenne_client_id");
        if (qsid.length() > 0 && qpass.length() > 0) {
          Serial.println("clearing eeprom");
          for (int i = 0; i < 96; ++i) { EEPROM.write(i, 0); }
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
              EEPROM.write(32+i, qpass[i]);
              Serial.print("Wrote: ");
              Serial.println(qpass[i]); 
            } 
              Serial.println("writing eeprom cayenne_userid:"); 
          for (int i = 0; i < qcuid.length(); ++i)
            {
              EEPROM.write(96+i, qcuid[i]);
              Serial.print("Wrote: ");
              Serial.println(qcuid[i]); 
            }    
            Serial.println("writing eeprom cayenne_passwd:"); 
          for (int i = 0; i < qcpass.length(); ++i)
            {
              EEPROM.write(160+i, qcpass[i]);
              Serial.print("Wrote: ");
              Serial.println(qcpass[i]); 
            }    
            Serial.println("writing eeprom cayenne_client_id:"); 
          for (int i = 0; i < qcclid.length(); ++i)
            {
              EEPROM.write(224+i, qcclid[i]);
              Serial.print("Wrote: ");
              Serial.println(qcclid[i]);
            }
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
    
    server.on("/cleareeprom", []() {
      content = "<!DOCTYPE HTML>\r\n<html>";
      content += "<p>Clearing the EEPROM</p></html>";
      server.send(200, "text/html", content);
      Serial.println("clearing eeprom");
      for (int i = 0; i < 288; ++i) { EEPROM.write(i, 0); }
      EEPROM.commit();
      WiFi.disconnect();
      WiFi.softAPdisconnect(false);  // Turn off  wifi Accesspoint after 0ne hour
      WiFi.enableAP(false);
      softap = 0;
      delay(500);
      ESP.restart();
    });
    server.on("/reset", []() {
      content = "<!DOCTYPE HTML>\r\n<html>";
      content += "<p>Rebooting ESP8266-IOT</p></html>";
      server.send(200, "text/html", content);
      Serial.println("....");
      delay(500);
      WiFi.disconnect();
      WiFi.softAPdisconnect(false);  // Turn off  wifi Accesspoint after 0ne hour
      WiFi.enableAP(false);
      ESP.restart();
    });
  }
}

void loop() 
{
   //if (softap == 1) { Cayenne.loop(); }
   if (WiFi.status() == WL_CONNECTED) { Cayenne.loop(); }
  server.handleClient();
}

// Les fonctions de cayenne called by  Cayenne.loop
CAYENNE_IN(8)  
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


CAYENNE_OUT(V1)
{
     // Lecture de la température en Celcius
     char str_t[5];     //result string 5 positions + \0 at the end
     t = dht.readTemperature();
     // converti float to string type 
     // format 5 positions with 2 decimal places
     //
     dtostrf(t, 4, 2, str_t );
     //
    Cayenne.virtualWrite(V1, (str_t), TYPE_TEMPERATURE, UNIT_CELSIUS);
}

CAYENNE_OUT(V2)
{
  //. Lecture  Humidite 
  // Send the Humidity value to Cayenne
    h = dht.readHumidity();
    
    Cayenne.virtualWrite(V2, (h), TYPE_RELATIVE_HUMIDITY, UNIT_PERCENT);
}
CAYENNE_OUT(V3)  //input  opto coupleur
{
  val = digitalRead(input);                      // read the input pin 
  Cayenne.virtualWrite(V3 , val, TYPE_DIGITAL_SENSOR, UNIT_DIGITAL);
}

// Fonction Cayenne optionnel pour lire l'etat du relay si vous avez brancher la sortie sur (relay_state)
//CAYENNE_OUT(V4) 
//{
//  int val = digitalRead(relay_state);                      // loop relay state read, read the input pin 
//  Cayenne.virtualWrite(V4 , val, TYPE_DIGITAL_SENSOR, UNIT_DIGITAL);
//}
