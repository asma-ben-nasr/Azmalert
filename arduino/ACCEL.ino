#include <ArduinoBLE.h>
#include "Arduino_BHY2.h"
#include "Nicla_System.h"

using namespace nicla;

const int UPDATE_FREQUENCY = 5;     // Update frequency in ms

int previousAcceleration = 0;

long previousMillis = 0;


SensorXYZ accel(31);

BLEService accelerationService("181A"); // Standard Acceleration Sensing service

// create characteristics
BLEIntCharacteristic accelCharacteristic("2A6E",               // Standard 16-bit Acceleration characteristic
                                        BLERead | BLENotify); // Remote clients can read and get updates


void setup() {
  Serial.begin(115200);
  while (!Serial);

  // begin initialization
  Serial.begin(115200);
  BHY2.begin();
  nicla::begin();
  
  accel.configure(200, 0);

  // set the local name peripheral advertises
  BLE.setLocalName("ACCEL");
  
  // set the UUID for the service this peripheral advertises:
  BLE.setAdvertisedService(accelerationService); // Advertise acceleration service

  // add the characteristics to the service
  accelerationService.addCharacteristic(accelCharacteristic);     // Add acceleration characteristic
 
  // add the service
  BLE.addService(accelerationService);

  accelCharacteristic.setValue(0);     // Set initial acceleration value

  
  // start advertising
  BLE.advertise();

  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {

  unsigned long acceleration_reading;

  BLEDevice central = BLE.central();  // Wait for a BLE central to connect
   
  // If central is connected to peripheral
  if (central) {
    Serial.println("Central connected");

    while (central.connected()) {
      BHY2.update();
      long currentMillis = millis();
            
      if (currentMillis - previousMillis >= UPDATE_FREQUENCY) {
                previousMillis = currentMillis;
                updateReadings();
            }
    }
    Serial.println("Central disconnected");
  }

}


int getAcceleration() {
    // Get acceleration as signed 16-bit int for BLE characteristic
    return (int) accel.z();
}



void updateReadings() {
    
    int acceleration_value = getAcceleration();
    

    if (acceleration_value != previousAcceleration) { // If reading has changed
        Serial.print("Acceleration: ");
        Serial.println(acceleration_value);
        accelCharacteristic.writeValue(acceleration_value); // Update characteristic
        previousAcceleration = acceleration_value;          // Save value
    }

    
   

}
