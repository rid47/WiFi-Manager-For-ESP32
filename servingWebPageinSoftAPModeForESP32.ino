// Serving ssid and password page in softAP mode


#include <WiFi.h>
#include <FS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

//Including html pages

#include "index.h" 
#include "index2.h"

const char* ssid="mushroom";
const char* password="admin123";
 
AsyncWebServer server(80);
 

String s=MAIN_page;
String s2=MAIN_page2;

void setup(){
  Serial.begin(115200);
 
  
   WiFi.softAP(ssid,password);

 

    Serial.println(WiFi.softAPIP());
    
    
    server.on("/html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", s);
  });

    
    server.on("/form", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", s2);
  });
 
  server.begin();

}
 
void loop(){
}
