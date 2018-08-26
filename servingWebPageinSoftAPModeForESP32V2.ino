// Serving ssid and password page in softAP mode

#include <WiFi.h>
#include <FS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

//Including html pages

#include "index.h" 
#include "index2.h"

const char* APssid="mushroom";
const char* APpassword="admin123";

const char* PARAM1= "ssid";
const char* PARAM2= "password";

String message1,message2;
 
AsyncWebServer server(80);
 

String s=MAIN_page;
String s2=MAIN_page2;

void setup(){
  Serial.begin(115200);
  WiFi.softAP(APssid,APpassword);
  Serial.println(WiFi.softAPIP());
    
    
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", s);
  });

    
    server.on("/form", HTTP_GET, [](AsyncWebServerRequest *request){


        
        if (request->hasParam(PARAM1)) {
            message1 = request->getParam(PARAM1)->value();
            Serial.println(message1);
        } else {
            message1 = "No message sent";
        }  

        if (request->hasParam(PARAM2)) {
            message2 = request->getParam(PARAM2)->value();
            Serial.println(message2);
        } else {
            message2 = "No message sent";
        }  
    request->send(200, "text/html", s2);
  });
 
  server.begin();
  
//  if(message1!='\0' && message2!='\0'){
//
//     Serial.println("Store messages to EEPROM");
//     Serial.println("connect to wifi using wifi credentials saved in EEPROM");
     /*const char* STssid=message1;
     const char* STpassword=message2;
     WiFi.begin(STssid,STpassword);
 
     while (WiFi.status() != WL_CONNECTED) {
     delay(1000);
     Serial.println("Connecting to WiFi..");
     */

//   Serial.println("Connected");
//   Serial.println(WiFi.localIP());
//   WiFi.mode(WIFI_STA);
//  }
   
    
    }


 
void loop(){
}
