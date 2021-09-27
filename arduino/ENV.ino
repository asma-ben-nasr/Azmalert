#include <ArduinoBLE.h>
#include "Arduino_BHY2.h"
#include "Nicla_System.h"

using namespace nicla;

const int UPDATE_FREQUENCY = 2000;     // Update frequency in ms

int previousTemperature = 0;
unsigned int previousHumidity = 0;
unsigned int previousGas = 0;
long previousMillis = 0;


Sensor temp(SENSOR_ID_TEMP);
Sensor hum(SENSOR_ID_HUM);
Sensor gas(SENSOR_ID_GAS);

BLEService environmentService("181B"); // Standard Environmental Sensing service

// create characteristics
BLEIntCharacteristic tempCharacteristic("2A61",               // Standard 16-bit Temperature characteristic
                                        BLERead | BLENotify); // Remote clients can read and get updates

BLEUnsignedIntCharacteristic humidCharacteristic("2A62", // Unsigned 16-bit Humidity characteristic
                                                 BLERead | BLENotify);

BLEUnsignedIntCharacteristic gasCharacteristic("2A63",               // Unsigned 32-bit Gas characteristic
                                                    BLERead | BLENotify); // Remote clients can read and get updates


void setup() {
  Serial.begin(115200);
  while (!Serial);

  // begin initialization
  Serial.begin(115200);
  BHY2.begin();
  nicla::begin();
  
  temp.configure(100, 0);
  hum.configure(100, 0);
  gas.configure(100, 0);
 



  // set the local name peripheral advertises
  BLE.setLocalName("ENV");
  
  // set the UUID for the service this peripheral advertises:
  BLE.setAdvertisedService(environmentService); // Advertise environment service

  // add the characteristics to the service
  environmentService.addCharacteristic(tempCharacteristic);     // Add temperature characteristic
  environmentService.addCharacteristic(humidCharacteristic);    // Add humidity characteristic
  environmentService.addCharacteristic(gasCharacteristic); // Add gas characteristic

  // add the service
  BLE.addService(environmentService);

  tempCharacteristic.setValue(0);     // Set initial temperature value
  humidCharacteristic.setValue(0);    // Set initial humidity value
  gasCharacteristic.setValue(0); // Set initial pressure value
  
  // start advertising
  BLE.advertise();

  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {

  unsigned long temperature_reading;
  unsigned long gas_reading;
  unsigned long humidity_reading;
  unsigned long iaq_index;

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


int getTemperature() {
    // Get temperature as signed 16-bit int for BLE characteristic
    return (int) temp.value();
}

unsigned int getHumidity() {
    // Get humidity as unsigned 16-bit int for BLE characteristic
    return (unsigned int) hum.value();
}

unsigned int getGas() {
    // Get gas as unsigned 32-bit int for BLE characteristic
    return (unsigned int) gas.value();
}

void updateReadings() {
    
    int temperature_value = getTemperature();
    unsigned int humidity_value = getHumidity();
    unsigned int gas_value = getGas();

    if (temperature_value != previousTemperature) { // If reading has changed
        Serial.print("Temperature: ");
        Serial.println(temperature_value);
        tempCharacteristic.writeValue(temperature_value); // Update characteristic
        previousTemperature = temperature_value;          // Save value
    }

    if (humidity_value != previousHumidity) { // If reading has changed
        Serial.print("Humidity: ");
        Serial.println(humidity_value);
        humidCharacteristic.writeValue(humidity_value);
        previousHumidity = humidity_value;
    }

    if (gas_value != previousGas) { // If reading has changed
        Serial.print("Gas: ");
        Serial.println(gas_value);
        gasCharacteristic.writeValue(gas_value);
        previousGas = gas_value;
    }

}
