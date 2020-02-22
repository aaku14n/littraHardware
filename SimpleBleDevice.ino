// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Sketch shows how to use SimpleBLE to advertise the name of the device and change it on the press of a button
// Useful if you want to advertise some sort of message
// Button is attached between GPIO 0 and GND, and the device name changes each time the button is pressed

#include "BluetoothSerial.h"
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRutils.h>
#include <Arduino.h>
#include <string.h>
#include <EEPROM.h>


const uint8_t PIN_IR_TX = 32;
const String ON = "on";
const String OFF = "off";
const String IR_STRING = "ir";
const String RENAME_COMMAND = "rename";
const int memoryAddress = 0;
const String DEFAULT_NAME = "LITTRA SMART";
const int EEPROM_SIZE = 128;


IRsend irTX(PIN_IR_TX);



#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif


BluetoothSerial SerialBT;
const int LED_BUILTIN = 26;


void setup() {

    Serial.begin(115200);
    Serial.setDebugOutput(true);
    pinMode(0, INPUT_PULLUP);

    // initialize EEPROM with predefined size

    EEPROM.begin(EEPROM_SIZE);

    
    Serial.println(ESP.getSdkVersion());
    String deviceName = read_String(memoryAddress);
    if(!deviceName || !deviceName[0] ){
      deviceName = DEFAULT_NAME;
      }
    Serial.println(deviceName);
    SerialBT.begin(deviceName);
    SerialBT.setTimeout(100);
    
    Serial.println("Press the button to change the device's name");
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH); 
    irTX.begin();
}


void handleIRCode(String code){
   Serial.println(code);
    String irCode  = code.substring(3,code.length());
    String finalString = "0x" + irCode;
    char irc[16];
    String irs = finalString;
    irs.toCharArray(irc, 16);
    unsigned long iri = strtoul(irc,0,16);
    Serial.println(iri);
    Serial.println(iri,HEX);
    irTX.sendJVC(iri, 16, true); 

  }


void writeString(int add,String data)
  {
    int _size = data.length();
    int i;
    for(i=0;i<_size;i++)
    {
      EEPROM.write(add+i,data[i]);
    }
    EEPROM.write(add+_size,'\0');   //Add termination null character for String Data
    EEPROM.commit();
  }
 
 
String read_String(int add)
  {
    int i;
    char data[50]; //Max 100 Bytes
    int len=0;
    unsigned char k;
    k=EEPROM.read(add);
    while(k != '\0' && len< 50)   //Read until null character
    {    
      k=EEPROM.read(add+len);
      data[len]=k;
      len++;
    }
    data[len]='\0';
    return String(data);
  }
void handleRename(String code){
  Serial.println(code);
   String newName  = code.substring(7,code.length());
   Serial.println(newName);
   writeString(memoryAddress,newName);
   ESP.restart();
  }
void loop() {

    String code = SerialBT.readString();
    code.trim();
   
    if(code == ON){
      digitalWrite(LED_BUILTIN, HIGH);
    }else if(code == OFF){
      digitalWrite(LED_BUILTIN, LOW);   
    }else if (code.indexOf(IR_STRING) >= 0){
      handleIRCode(code);
     }else if(code.indexOf(RENAME_COMMAND) >= 0){
      handleRename(code);
      }
    
    yield();
}
