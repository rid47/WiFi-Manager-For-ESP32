//-----------------------Including library------------------------------//
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ticker.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>        // https://github.com/tzapu/WiFiManager
int Version=1;

//--------------------ISR for implementing watchdog-------------------//
Ticker secondTick;
volatile int watchdogCount=0;
void ISRwatchdog(){

watchdogCount++;
if(watchdogCount==600){


    Serial.println();
    Serial.print("The watch dog bites......");
    //ESP.reset();
    ESP.restart();
  }
}

//-------------------Defining required variables---------------------//

String pirStatus="0";
String pir1Status="0";
String pir2Status="0";
String Status;
char msg[4];
int pirValue1=0;
int pirValue2=0;
int calibrationTime = 15;
long unsigned int lowIn1;
long unsigned int lowIn2;
long unsigned int lowIn3;
         

//the amount of milliseconds the sensor has to be low 
//before we assume all motion has stopped

long unsigned int pause =15000;  
long unsigned int pause2 =15000; //publish data after this interval while both PIR read zero

boolean lockLow1 = true;
boolean takeLowTime1;  

boolean lockLow2 = true;
boolean takeLowTime2;  

//-----------------Defining required pins---------------------------//

#define pirPin1 14             // NODE MCU PIN D5
#define pirPin2 12             // NODE MCU PIN D6

//---------------------------Broker IP-----------------------------------//

const char* mqtt_server = "182.163.112.207";
WiFiClient espClient;
PubSubClient client(espClient);

//---------------------------------------------setup-------------------//


void setup() {
  
Serial.begin(115200);
secondTick.attach(1,ISRwatchdog);
WiFiManager wifiManager;
//wifiManager.resetSettings();
wifiManager.autoConnect("Smart Home", "admin1234");
Serial.println("Connected.");

pinMode(pirPin1,INPUT);
pinMode(pirPin2,INPUT);

//setup_wifi();
client.setServer(mqtt_server, 1883);
client.setCallback(callback);

//give the sensor some time to calibrate
Serial.print("calibrating sensor ");
for(int i = 0; i < calibrationTime; i++){
      Serial.print(".");
      delay(1000);
      }
    Serial.println(" done");
    Serial.println("SENSORs are ACTIVE");
    delay(50);



}


//-------------------------------------------loop-----------------------------------------//

void loop()
{

  watchdogCount=0;

 if (!client.connected()) {
    reconnect();
  }
  client.loop();

 //---------------------Updating device status---------------------------------------------//

    if(Version==1)
    {
      Serial.println("Firmware v17.0");
      Version++;
    }
    

    Status="";
    Status=Status+pir1Status+pir2Status;
    Status.toCharArray(msg,4);
    delay(250);
   



  pirValue1=digitalRead(pirPin1);// Reading sesor data
  pirValue2=digitalRead(pirPin2);
  
  delay(100);
  Serial.print("pirValue1:");
  Serial.println(pirValue1);
  Serial.print("pirValue2:");
  Serial.println(pirValue2);

  if(pirValue1== HIGH)
  {
        lockLow1 = true;
        
        //digitalWrite(lightPin2,LOW);

    if(lockLow1){  
         //makes sure we wait for a transition to LOW before any further output is made:
         lockLow1 = false;            
         Serial.println("---");
         Serial.print("motion detected by DOOR PIR at ");
         Serial.print(millis()/1000);
         Serial.println(" sec");

         // Making Indicator pin HIGH

         //digitalWrite(pirIndicator1,HIGH);
         pir1Status="1"; 
         delay(50);
         }         
         takeLowTime1 = true;
       }
  
  
  
  if(pirValue1==LOW)

  {

       
        if(takeLowTime1){
        lowIn1 = millis();          //save the time of the transition from high to LOW
        takeLowTime1 = false;       //make sure this is only done at the start of a LOW phase
        }
       //if the sensor is low for more than the given pause, 
       //we assume that no more motion is going to happen
       if(!lockLow1 && millis() - lowIn1 > pause){  
           //makes sure this block of code is only executed again after 
           //a new motion sequence has been detected
           lockLow1 = true;                        


        
        //digitalWrite(lightPin,HIGH);
        
           
        Serial.print("motion ended detected by DOOR PIR at ");
        Serial.print(millis()/1000);
        Serial.println(" sec");
        // Making Indicator pin HIGH
        //digitalWrite(pirIndicator1,LOW);
        pir1Status="0";
        
        delay(50);
           }
       }


//----------------------------Reading Second PIR------------------------------------------//
   if(pirValue2== HIGH)
  {
        lockLow2 = true;
        
        //digitalWrite(lightPin2,LOW);

    if(lockLow2){  
         //makes sure we wait for a transition to LOW before any further output is made:
         lockLow2 = false;            
         Serial.println("---");
         Serial.print("motion detected by CHAIR PIR at ");
         Serial.print(millis()/1000);
         Serial.println(" sec");

         // Making Indicator pin HIGH

         //digitalWrite(pirIndicator2,HIGH);
         pir2Status="1"; 
         delay(50);
         }         
         takeLowTime2 = true;
       }
  
  
  
  if(pirValue2==LOW)

  {

       
        if(takeLowTime2){
        lowIn2 = millis();          //save the time of the transition from high to LOW
        takeLowTime2 = false;       //make sure this is only done at the start of a LOW phase
        }
       //if the sensor is low for more than the given pause, 
       //we assume that no more motion is going to happen
       if(!lockLow2 && millis() - lowIn2 > pause){  
           //makes sure this block of code is only executed again after 
           //a new motion sequence has been detected
           lockLow2 = true;                        


        
                
           
        Serial.print("motion end detected by CHAIR PIR at ");
        Serial.print(millis()/1000);
        Serial.println(" sec");
        // Making Indicator pin LOW
        //digitalWrite(pirIndicator2,LOW);
        pir2Status="0";
        delay(50);
           }
       } 
          
//-----------------------------------Sending Data to broker based on pirStatus-------------------------------------//
       
  
  if(pir1Status== "1" || pir2Status== "1")
  {
        pirStatus="1";
        client.publish("/relay/0/set","1");
        Serial.print("pirstatus:");
        Serial.println(pirStatus);
  }
  
  else
  {

  
  pirStatus="0";
  Serial.print("pirStatus:");
  Serial.println(pirStatus);
  lowIn3=millis();
  if(millis()-lowIn3>pause2){
  
  client.publish("/relay/0/set","0");
  Serial.print("Published data to turn off light");
  }}}
  



//-------------------------------------reconnect()---------------------------------------------//

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //if your MQTT broker has clientID,username and password
    //please change following line to    if (client.connect(clientId,userName,passWord))
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
     //once connected to MQTT broker, subscribe command if any

//---------------------------------------Subscribing to required topics-----------------------//
      client.subscribe("apartment/login");
      Serial.println("Subsribed to topic: apartment/login");


    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 6 seconds before retrying
      delay(5000);
    }
  }
} //end reconnect()


//---------------------------Callback funtion-------------------------------------//

void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

//-------------------------Publishing device status upon request from user app--------------------------//

  
  if(strcmp(topic, "apartment/login") == 0){
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    char data2=payload[i];


    if (data2=='1')
    {
      client.publish("apartment/sensorStatus",msg);
      Serial.print("Published sensor Status:");
      Serial.println(msg);
    }}}

//----------------------------------Restarting the board from Engineer's end----------------------------//

  if(strcmp(topic, "apartment/reset") == 0){
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    char data3=payload[i];


    if (data3=='1')
    {

      Serial.println("Resetting Device.........");
       ESP.restart();
      }}}}// End of callback function





