**OVERVIEW**

**These two arduino files are responsible for initializing and running the arduino and BLE shield for the FrideCart Project.**

**The arduino and shield acts as a slave to the mobile device running the app. They simply broadcast information about the cart's sensors.**

**The "FridgeCartArduino.ino" is the code responsible for the following:**

-Initializing the Arduino

-Establishing connection to shield and applies configuration

-Performing sanity checks, code initializations, and device initializations

-Setting the Baud rate

-Implementing GATT services and characteristics

-Performing cart sensor operations:

  -Converting sensor read values to expected values
  
  -Converting expected values to byte arrays
  
  -Setting characteristic values to the byte array value
  
-Broadcasting services and characteristics

**The "BlueFruitConfig.h" does the following:**

-Configures Adafruit Bluefruit LE shield settings

-Sets up communication between arduino and shield

-Sets the shield's communication pins and types

-Establishes read buffer size

*It's best to leave this alone unless you want to change the communication type, the hardware used, or the read buffer

**The "SensorDebug.txt" can be loaded onto the arduino as a .ino to check the sensor values being read by the arduino**


**HOW IT WORKS**

**Terms:**

-Bluetooth Low Energy (BLE) is a form of the original Bluetooth communication protocol intended "to provide considerably reduced power consumption and cost while maintaining a similar communication range".

-The Attribute Protocol (ATT) is a data protocol which stores Services, Characterisitics, and other data in a lookup table using 16-bit (or larger) IDs, these ID's are know as UUIDs.

-A Universaly Unique Identifier (UUID) is a 16 or 128-bit ID associated with each Service or Characteristic. 

-The Generic Attribute Profile (GATT) is built on top of the ATT and provides a standardized way for Bluetooth devices to discover and interact with each other's services and attributes.

-A GATT Service is a collection of Characteristics that serve a specific function. 

-A GATT Characteristic is a data structure consisting of various attributes which define the object's properties, permissions, payload, and description. It is what gets transmitted by Bluetooth systems.

-A MAC address is a unique identifier that every network interfacing controller (arduino/shield/computer/phone) has. MAC addresses are read-only and should not, or cannot, be changed.

**Our System:**

The FridgeCart's system consists of an Arduino Uno R3 and an Adafruit Bluefruit LE Shield. The Arduino directly connects to the carts electrical systems and collects data on them. The arduino and shield take this data, refine and package it into characteristics, and then broadcast it to whoever it is connected to (the mobile app).

First, the system will broadcast an special type of information known as an advertisement. When this broadcast is picked up by another Bluetooth device, it can be used to form a connection between the two devices. This process can be customized in various ways, but is left mostly untouched in this implementation. Advertisements carry various types of information about the sender, such as its MAC address.

Next, we use the mobile app to connect to the system via MAC filtering. Essentially, the app will only display advertisers which contain a specifc MAC address.

Once connected, the mobile app and system can send and receive information between each other.

Further filtering ensures that only relevant data is diplayed to the user.







