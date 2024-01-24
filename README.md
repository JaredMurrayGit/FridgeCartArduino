These two arduino files are responsible for initializing and running the arduino and BLE shield for the FrideCart Project.

The arduino and shiled acts as a slave to the mobile device running the app. They simply broadcast information about the cart's sensors.

The "FridgeCartArduino.ino" is the code responsible for the following:
-Initializing the Arduino
-Establishing connection to shield and applies configuration
-Performing sanity checks, code initializations, and device initializations
-Setting the Baud rate
-Implementing GATT services and characteristics
-Performing cart sensor operations:
  >Converting sensor read values to expected values
  >Converting expected values to byte arrays
  >Setting characteristic values to the byte array value
-Broadcasts services and characteristics

The "BlueFruitConfig.h" does the following:
-Configures Adafruit Bluefruit LE shield settings
-Sets up communication between arduino and shield
-Sets the shield's communication pins and types
-Establishes read buffer size
*It's best to leave this alone unless you want to change the communication type, the hardware used, or the read buffer

The "SensorDebug.txt" can be loaded onto the arduino as a .ino to check the sensor values being read by the arduino
