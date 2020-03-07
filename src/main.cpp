#include <Arduino.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "WiFi.h"
#include <math.h>

#define SERVICE_UUID    "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"      
 // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
int value = 0;
float y;
unsigned long t;
class MyServerCallBacks: public BLEServerCallbacks{
  void onConnect(BLEServer *pServer){
    deviceConnected = true;
  }
  void onDisconnect (BLEServer *pServer){
    deviceConnected = false;
  }
};
class MyCallBack: public BLECharacteristicCallbacks{
  void onWrite(BLECharacteristic *pCharacteristic){
    std::string rxValue = pCharacteristic->getValue();

    if(rxValue.length() > 0){
        Serial.println("*************");
        Serial.print("Received Value: ");
        
        for (int i = 0; i < rxValue.length(); i++)
        {
          Serial.print(rxValue[i]);
        }
    }
    Serial.println();
  }
};
void setup() {
  WiFi.mode(WIFI_OFF);
  Serial.begin(9600);

  BLEDevice::init("HEATH_DEVICE");

  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallBacks);

  BLEService *pService = pServer->createService(SERVICE_UUID);
  
  
  //tao Characteristic co UUID va tinh chat
  pCharacteristic = pService -> createCharacteristic(
  CHARACTERISTIC_UUID_TX,
  BLECharacteristic::PROPERTY_NOTIFY
  );
  pCharacteristic->addDescriptor(new BLE2902());

  //tao Characteristic co UUID va tinh chat
  BLECharacteristic *pCharacteristic = pService -> createCharacteristic(
    CHARACTERISTIC_UUID_RX,
    BLECharacteristic::PROPERTY_WRITE_NR
  );
  //tao Callback cho Characteristic
  pCharacteristic -> setCallbacks(new MyCallBack());

  pService -> start();
  pServer  -> getAdvertising() ->start();

  //Serial.println("Waiting");
}

void loop() {
  //lay thoi gian bat dau chay cua vxl
  t = millis();
  //dung thoi gian tinh ham sine
  y = sin(0.01*PI*t);

  std::string StringTime((char*)&t,4); 
  std::string StringData((char*)&y,4);

  pCharacteristic -> setValue(StringData+StringTime);
  pCharacteristic -> notify();
  Serial.println(y);
}