#include <ArduinoBLE.h>
#include "Arduino_BHY2.h"
#include "Nicla_System.h"
const int UPDATE_FREQUENCY = 5;
long previousMillis = 0;
using namespace nicla;
SensorXYZ accelerometer(31);
Sensor hum(131);
BLEService tempService("181A"); // create service
// create characteristic and allow remote device to read and write
BLEIntCharacteristic tempCharacteristic("2A6E",  BLERead | BLENotify);
// create characteristic and allow remote device to get notifications and read the value


void setup() {
  Serial.begin(115200);
  while (!Serial);

  // begin initialization
 
  BHY2.begin();
  nicla::begin();
   
   accelerometer.configure(200, 0);


  // set the local name peripheral advertises
  BLE.setLocalName("ACCEL");
  // set the UUID for the service this peripheral advertises:
  BLE.setAdvertisedService(tempService);

  // add the characteristics to the service
  tempService.addCharacteristic(tempCharacteristic);

  // add the service
  BLE.addService(tempService);

  tempCharacteristic.writeValue(0);
  // start advertising
  BLE.advertise();

  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {

  BLEDevice central = BLE.central();  // Wait for a BLE central to connect

  // If central is connected to peripheral
  if (central) {
    Serial.println("Central connected");

    while (central.connected()) {
       BHY2.update();
      long currentMillis = millis();
      // Check temperature & humidity with UPDATE_FREQUENCY
      if (currentMillis - previousMillis >= UPDATE_FREQUENCY) {
        previousMillis = currentMillis;
       
        
        Serial.println(accelerometer.z());
    
        tempCharacteristic.writeValue(accelerometer.z());
      }
    }
    Serial.println("Central disconnected");
  }

}
