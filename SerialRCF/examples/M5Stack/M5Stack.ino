#include <M5Stack.h>
#include "SerialRCF.h"

unsigned long timeLabel = 0;
int receiveData[128] = {}; 

void setup() {
  M5.begin();
  M5.Lcd.setTextSize(4);
  Serial.begin(9600);
}

void loop() {

  SerialRCF.Receive();
  receiveData[0] = SerialRCF.ReceiveData[0];
  receiveData[1] = SerialRCF.ReceiveData[1];
  receiveData[2] = SerialRCF.ReceiveData[2];

  if (200 < millis() - timeLabel)
  {
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.printf("%d\n", receiveData[0]);
    M5.Lcd.printf("%d\n", receiveData[1]);
    M5.Lcd.printf("%d\n", receiveData[2]);

    SerialRCF.Send(receiveData, 6);
    timeLabel = millis();
  }
  
}


