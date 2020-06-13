/*
  SimpleEspNowConnectionClient

  Basic EspNowConnection Client implementation

  HOWTO Arduino IDE:
  - Prepare two ESP8266 or ESP32 based devices (eg. WeMos)
  - Start two separate instances of Arduino IDE and load 
    for the first one the 'SimpleEspNowConnectionServer.ino' and
    for the second one the 'SimpleEspNowConnectionClient.ino' sketch and upload 
    these to the two ESP devices.
  - Start the 'Serial Monitor' in both instances and set baud rate to 9600
  - Type 'startpair' into the edit box of both 'Serial Monitors' and hit Enter key (or press 'Send' button)
  - After devices are paired, type 'send' or 'multisend' into the edit box  
    of the 'Serial Monitor' and hit Enter key (or press 'Send' button)

  - You can use multiple clients which can be connected to one server

  Created 04 Mai 2020
  By Erich O. Pintar
  Modified 17 Mai 2020
  By Erich O. Pintar

  https://github.com/saghonfly/SimpleEspNowConnection

*/


#include "SimpleEspNowConnection.h"

SimpleEspNowConnection simpleEspConnection(SimpleEspNowRole::CLIENT);

String inputString;
String serverAddress;
int multiCounter = -1;
char temp[2200];


char* getBigMessage()
{
  snprintf(temp, 2200,  
"\
SHRDZM Sensor Integration Platform\
SHRDZM (a unique name, no acronym) is a custom firmware for ESP8285/ESP8266 based sensors. It uses the Arduino Core for ESP8266 framework and a number of 3rd party libraries.\
\
It provides wiring digrams, source code and working examples. Also plug-ins for most common used home automation solutions will be collected.\
\
To build cheap integrations of many kind of sensors into your individual automation solution with the focus on energy efficiency (battery operated devices) and simpleness.\
\
Features\
Support for multiple sensor types\
All supported sensor types covered in one firmware\
Sensor type can be changed by configuration without flashing new firmware\
New firmware versions (eg. due to supporting new sensor types) can be upgraded by OTA\
To build cheap integrations of many kind of sensors into your individual automation solution with the focus on energy efficiency (battery operated devices) and simpleness.\
\
Features\
Support for multiple sensor types\
All supported sensor types covered in one firmware\
Sensor type can be changed by configuration without flashing new firmware\
New firmware versions (eg. due to supporting new sensor types) can be upgraded by OTA\
To build cheap integrations of many kind of sensors into your individual automation solution with the focus on energy efficiency (battery operated devices) and simpleness.\
\
Features\
Support for multiple sensor types\
All supported sensor types covered in one firmware\
Sensor type can be changed by configuration without flashing new firmware\
New firmware versions (eg. due to supporting new sensor types) can be upgraded by OTA\
To build cheap integrations of many kind of sensors into your individual automation solution with the focus on energy efficiency (battery operated devices) and simpleness.\
\
Features\
Support for multiple sensor types\
All supported sensor types covered in one firmware\
Sensor type can be changed by configuration without flashing new firmware\
New firmware versions (eg. due to supporting new sensor types) can be upgraded by OTA\
");  

  return temp;
}

void OnSendError(uint8_t* ad)
{
  Serial.println("SENDING TO '"+simpleEspConnection.macToStr(ad)+"' WAS NOT POSSIBLE!");
}

void OnMessage(uint8_t* ad, const char* message)
{
  Serial.println("MESSAGE:"+String(message)+" from server ("+simpleEspConnection.macToStr(ad)+")");
}

void OnNewGatewayAddress(uint8_t *ga, String ad)
{  
  Serial.println("New GatewayAddress '"+ad+"'");

  simpleEspConnection.setServerMac(ga);
}

void setup() 
{
  Serial.begin(9600);
  Serial.println();

  simpleEspConnection.begin(true);
//  simpleEspConnection.setPairingBlinkPort(2);  

   serverAddress = "ECFABCC08CDA"; // Test if you know the server
   simpleEspConnection.setServerMac(serverAddress);
  simpleEspConnection.onNewGatewayAddress(&OnNewGatewayAddress);    
  simpleEspConnection.onSendError(&OnSendError);  
  simpleEspConnection.onMessage(&OnMessage);  
  
  Serial.println(WiFi.macAddress());  
}

void loop() 
{
  simpleEspConnection.loop();

  if(multiCounter > -1)   //send a lot of messages but ensure that only send when it is possible
  {
    if(simpleEspConnection.canSend())
    {
      multiCounter++;

      simpleEspConnection.sendMessage((char *)String("MultiSend #"+String(multiCounter)).c_str());
            
      if(multiCounter == 50)  // stop after 50 sends
        multiCounter = -1;
    }
  }
  
  while (Serial.available()) 
  {
    char inChar = (char)Serial.read();
    if (inChar == '\n') 
    {
      Serial.println(inputString);

      if(inputString == "startpair")
      {
        simpleEspConnection.startPairing(30);
      }
      else if(inputString == "endpair")
      {
        simpleEspConnection.endPairing();
      }      
      else if(inputString == "changepairingmac")
      {
        uint8_t np[] {0xCE, 0x50, 0xE3, 0x15, 0xB7, 0x33};
        
        simpleEspConnection.setPairingMac(np);
      }      
      else if(inputString == "send")
      {
        if(!simpleEspConnection.sendMessage("This comes from the Client"))
        {
          Serial.println("SENDING TO '"+serverAddress+"' WAS NOT POSSIBLE!");
        }
      }
      else if(inputString == "bigsend")
      {
        if(!simpleEspConnection.sendMessage(getBigMessage()))
        {
          Serial.println("SENDING TO '"+serverAddress+"' WAS NOT POSSIBLE!");
        }
      }
      else if(inputString == "multisend")
      {
        multiCounter = 0;
      }
      
      inputString = "";
    }
    else
    {
      inputString += inChar;
    }
  }
}
