#include <ESP8266WiFi.h>        
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiManager.h>
#include <ArduinoJson.h> 
#include <WiFiClient.h>
WiFiManager wifiManager;
char Device_ID[20];
int smokeA0 = A0;
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
 wifiManager.resetSettings();
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
  //pinMode(smokeA0, INPUT);
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
void loop(){
	send();
	smoke();
}
	
//-------------------------------------------------------------------------------------------------------	
void smoke(){


int analogSensor = analogRead(smokeA0);
Serial.println(analogSensor);
if(WiFi.status()==WL_CONNECTED)
{
	const char* server = "http://api.thingspeak.com/update";
	String my_Api_key = "VV4KANZLJPWOUUIP";
	WiFiClient wifiClient;
	HTTPClient http;
	http.begin(wifiClient,server);
	// Specify content-type header
	http.addHeader("Content-Type", "application/x-www-form-urlencoded");
	String httpRequestData = "api_key=" + my_Api_key + "&field1=" + String(analogSensor);
	int httpResponseCode = http.POST(httpRequestData);
}

}
//-------------------------------------------------------------------------------------------------------
void send(){

if(WiFi.status()==WL_CONNECTED){
  digitalWrite(BUILTIN_LED, LOW);
  delay(1000); 
  digitalWrite(BUILTIN_LED, HIGH); 
  delay(1000);
}

if(WiFi.status()==WL_CONNECTED)
{
	const char* server = "http://api.thingspeak.com/update";
	String my_Api_key = "JU0DQTQ7J0YFJEP5";
	WiFiClient wifiClient;
	HTTPClient http;
	http.begin(wifiClient,server);
	// Specify content-type header
	http.addHeader("Content-Type", "application/x-www-form-urlencoded");
	String httpRequestData = "api_key=" + my_Api_key + "&field1=" + String(random(100));
	int httpResponseCode = http.POST(httpRequestData);
}
}

