//vendor space
#include <WiFiNINA.h>
#include <Arduino.h>
#include <Wire.h>

//config space
#include "configs.h"

//user space
#include "CHardwareImpl.h"
#include "CWifiClient.h"
#include "CPeopleCounter.h"

ele400::PeopleCounter impl;
void setup(void)
{
  Wire.begin();
  Wire.setClock(400000);
  
  Serial.begin(9600);
  while(!SerialUSB){}
  impl.init();  
}
void loop()
{
  Serial.println(impl.checkForEvent());
}

/*
ele400::CHardwareImpl impl;
ele400::CWiFiClient wifiClient(impl);

bool AnEventHasOccuredSinceLastUpdate = false;


unsigned long getTime()
{
  return WiFi.getTime();
}
void onMessageCallback(int sz)
{
  wifiClient.onMessageReceived(sz);
}




//setup
void setup(void)
{
  Wire.begin();

  // initialize the digital pin as an output.
  Serial.begin(9600);
  while(!SerialUSB){} // To be able to connect Serial monitor after reset and before first printout

  //initialize implementation and wifi client
  impl.init();
  wifiClient.init(onMessageCallback, getTime);
}
void loop(void)
{
  Serial.println(impl.jsonPayload());
  if(impl.doPplCycle())
  {
    Serial.println(impl.jsonPayload());
  }
  //AnEventHasOccuredSinceLastUpdate = AnEventHasOccuredSinceLastUpdate || impl.doPplCycle() || impl.doHandCycle();
  //AnEventHasOccuredSinceLastUpdate = wifiClient.loop(AnEventHasOccuredSinceLastUpdate);
}*/
