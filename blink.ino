
#include <ESP8266WiFi.h>        
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiManager.h>
#include <ArduinoJson.h> 
WiFiManager wifiManager;
char Device_ID[20];
//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());

}

void setup() {
 Serial.begin(115200);
 Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");
          strcpy(Device_ID, json["Device_ID"]);
        } else {
          Serial.println("failed to load json config");
        }
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
 if ( WiFi.status() != WL_CONNECTED ) {
  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);
 }

 WiFiManagerParameter custom_output("Device_ID", "Device_ID",Device_ID , 20);
 wifiManager.setSaveConfigCallback(saveConfigCallback);
 wifiManager.addParameter(&custom_output);
 wifiManager.autoConnect("AdvanceTech");
 strcpy(Device_ID, custom_output.getValue());
//if you get here you have connected to the WiFi
  Serial.println(F("WIFIManager connected!"));
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["Device_ID"] = Device_ID;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
    //end save
  }
  pinMode(BUILTIN_LED,OUTPUT);

  Serial.print(F("IP --> "));
  Serial.println(WiFi.localIP());
  Serial.print(F("GW --> "));
  Serial.println(WiFi.gatewayIP());
  Serial.print(F("SM --> "));
  Serial.println(WiFi.subnetMask());
 
  Serial.print(F("DNS 1 --> "));
  Serial.println(WiFi.dnsIP(0));
 
  Serial.print(F("DNS 2 --> "));
  Serial.println(WiFi.dnsIP(1));
}
 
void loop() {
String Stringone=String(Device_ID);
String Stringtwo="LOW";
String Stringthree="HIGH";
  if(WiFi.status()== WL_CONNECTED){
  digitalWrite(BUILTIN_LED, LOW);
  Serial.println(Stringone+Stringtwo);
  delay(1000); 
  digitalWrite(BUILTIN_LED, HIGH); 
  Serial.println(Stringone+Stringthree);
  delay(1000);
    }
}
