#include <Arduino.h>
#include <ArduinoOTA.h>
#include "wifi_conn.h"

#define SIGNAL_LED 19

const int ota_port = 1883;
const char* ota_username = "signalio";
const char* ota_password = "root1234";

wifiConn wifi;


void get_system_info(){
  Serial.print("SoC: ");
  Serial.println(SOC);
  Serial.printf("Firmware size: %i\n",ESP.getSketchSize());
  Serial.printf("Free space for firmware: %i\n",ESP.getFreeSketchSpace());
  Serial.printf("Heap size: %i\n",ESP.getHeapSize());
  Serial.printf("Free heap: %i\n",ESP.getFreeHeap());
  Serial.print("SDK version: ");
  Serial.println(ESP.getSdkVersion());
}


void signal_led(int time){
  digitalWrite(SIGNAL_LED, HIGH);
  delay(time);
  digitalWrite(SIGNAL_LED, LOW);
  delay(time);
}


void ota(){
  WiFi.mode(WIFI_STA);
    WiFi.begin();

    Serial.printf("OTA Begin\n");
    Serial.printf("Use your local IP adress to udate board");
    Serial.println(WiFi.localIP());

    ArduinoOTA.setHostname(ota_username);
    ArduinoOTA.setPassword(ota_password);
    ArduinoOTA.setPort(ota_port);
    Serial.println(ota_password);

    ArduinoOTA.onStart([](){
        String type;
        if(ArduinoOTA.getCommand() == U_FLASH){
            type = "scetch"; // firmware
        }else
        {
            type = "filesystem";
        }
        if(type == "filesystem"){
            SPIFFS.end();
        }
        Serial.println("Start updating" + type);
    });

    ArduinoOTA.onEnd([](){

        Serial.printf("\nEnd updating\n");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total){
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        signal_led(50);
    });

    ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
    });
    
    ArduinoOTA.begin();
}


void setup() {
  pinMode(SIGNAL_LED, OUTPUT);
  for (size_t i = 0; i < 5; i++)
  {
    signal_led(250);
  }
  Serial.begin(9600);
  get_system_info();

  if(!SPIFFS.begin()){
    Serial.println("Failed to init file system");
    signal_led(5000);
    ESP.restart();
  }

  Serial.println("File system inited");
  bool state = wifi.wifi_connect();

  if(state){
        Serial.println("Wi-Fi connected");
        wifi.printCurrentNet();
        wifi.printWifiData();
  }
  else{
      Serial.println("Wifi not connected");
      wifi.wifi_manager();
    }
  
  ota();
}


void loop() {
  ArduinoOTA.handle();
  yield();
}