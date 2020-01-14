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

const uint8_t PIN_IR_TX = 32;
const String ON = "on";
const String OFF = "off";
const String IR_STRING = "ir";

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
    Serial.print("ESP32 SDK: ");
    Serial.println(ESP.getSdkVersion());
    SerialBT.begin("ESP32 light");
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
//    Serial.println(iri->bits);
    irTX.sendJVC(iri, 16, true); 

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
     }
    
    yield();
}
