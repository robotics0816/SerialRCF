#include "SerialRCF.h"

unsigned long timeLabel = 0;
int receiveData[128] = {}; 

void setup() {
  Serial.begin(9600);
}

void loop() {

  SerialRCF.Receive();
  receiveData[0] = SerialRCF.ReceiveData[0];
  receiveData[1] = SerialRCF.ReceiveData[1];
  receiveData[2] = SerialRCF.ReceiveData[2];

  if (200 < millis() - timeLabel)
  {
    SerialRCF.Send(receiveData, 6);
    timeLabel = millis();
  }
  
}


