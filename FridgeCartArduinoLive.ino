
#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "Adafruit_BLEGatt.h"

#include "BluefruitConfig.h"

#if SOFTWARE_SERIAL_AVAILABLE
  #include <SoftwareSerial.h>
#endif


// Create the bluefruit object, either software serial...uncomment these lines
/*
SoftwareSerial bluefruitSS = SoftwareSerial(BLUEFRUIT_SWUART_TXD_PIN, BLUEFRUIT_SWUART_RXD_PIN);

Adafruit_BluefruitLE_UART ble(bluefruitSS, BLUEFRUIT_UART_MODE_PIN,
                      BLUEFRUIT_UART_CTS_PIN, BLUEFRUIT_UART_RTS_PIN);
*/

/* ...or hardware serial, which does not need the RTS/CTS pins. Uncomment this line */
// Adafruit_BluefruitLE_UART ble(BLUEFRUIT_HWSERIAL_NAME, BLUEFRUIT_UART_MODE_PIN);

/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

/* ...software SPI, using SCK/MOSI/MISO user-defined SPI pins and then user selected CS/IRQ/RST */
//Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_SCK, BLUEFRUIT_SPI_MISO,
//                             BLUEFRUIT_SPI_MOSI, BLUEFRUIT_SPI_CS,
//                             BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

Adafruit_BLEGatt gatt(ble);

// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

/* The service information */

int32_t dataServiceId;
int32_t tempMeasureCharId;
int32_t solMeasureCharId;
int32_t battMeasureCharId;


#define RT0 10000 // Ω 
#define B 3950 // K 
#define VCC 5 //Supply voltage 
#define R 10000 //R=10KΩ 
double RT, VR, ln, TX, T0, VRT, VIN1, VOUT1, VIN0, VOUT0, Percent, Low_Battery; 

/**************************************************************************/
/*!
    @brief  Sets up the HW an the BLE module (this function is called
            automatically on startup)
*/
/**************************************************************************/
void setup(void)
{
  while (!Serial); // required for Flora & Micro
  delay(500);

  boolean success;

  Serial.begin(115200);
  Serial.println(F("Adafruit Bluefruit Fridge Cart Example"));
  Serial.println(F("--------------------------------------------"));

  randomSeed(micros());

  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  /* Perform a factory reset to make sure everything is in a known state */
  Serial.println(F("Performing a factory reset: "));
  if (! ble.factoryReset() ){
       error(F("Couldn't factory reset"));
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  T0 = 25 + 273.15;

  uint8_t serviceUUID[] ={0x93,0x7B,0x64,0xAA,0x0C,0xBB,0x40,0x4D,0x96,0xAC,0x99,0xF8,0xA7,0xA5,0xB0,0x34};
 
  Serial.println(F("Adding the Custom Monitoring Service definition (UUID = 937B64AA0CBB404D96AC99F8A7A5B034): "));
  dataServiceId = gatt.addService(serviceUUID);
  if (dataServiceId == 0) {
    error(F("Could not add Data service"));
  }

  uint8_t battCharUUID[] ={0x93,0x7B,0x65,0xAB,0x0C,0xBB,0x40,0x4D,0x96,0xAC,0x99,0xF8,0xA7,0xA5,0xB0,0x34};
  const char* charDescription = "This is a sample characteristic";

  Serial.println(F("Adding the Battery Measurement characteristic (UUID = 937B65AB0CBB404D96AC99F8A7A5B034): "));
  battMeasureCharId = gatt.addCharacteristic(battCharUUID, GATT_CHARS_PROPERTIES_INDICATE, 8, 8, BLE_DATATYPE_BYTEARRAY);
  if (battMeasureCharId == 0) {
    error(F("Could not add Battery characteristic"));
  }

  uint8_t tempCharUUID[] ={0x93,0x7B,0x65,0xAC,0x0C,0xBB,0x40,0x4D,0x96,0xAC,0x99,0xF8,0xA7,0xA5,0xB0,0x34};
  
  Serial.println(F("Adding the Temperature Measurement characteristic (UUID = 937B65AC0CBB404D96AC99F8A7A5B034): "));
  tempMeasureCharId = gatt.addCharacteristic(tempCharUUID, GATT_CHARS_PROPERTIES_INDICATE, 8, 8, BLE_DATATYPE_BYTEARRAY);
  if (tempMeasureCharId == 0) {
    error(F("Could not add Temperature characteristic"));
  }

  uint8_t solCharUUID[] ={0x93,0x7B,0x65,0xAD,0x0C,0xBB,0x40,0x4D,0x96,0xAC,0x99,0xF8,0xA7,0xA5,0xB0,0x34};

  Serial.println(F("Adding the Solar Measurement characteristic (UUID = 937B65AD0CBB404D96AC99F8A7A5B034): "));
  solMeasureCharId = gatt.addCharacteristic(solCharUUID, GATT_CHARS_PROPERTIES_INDICATE, 8, 8, BLE_DATATYPE_BYTEARRAY);
  if (solMeasureCharId == 0) {
    error(F("Could not add Solar characteristic"));
  }


  /* Reset the device for the new service setting changes to take effect */
  Serial.print(F("Performing a SW reset (service changes require a reset): "));
  ble.reset();

  Serial.println();
}

/** Send randomized heart rate data continuously **/
void loop(void)
{
  //double temp = random(0, 100) / 10.0;

  //Thermistor values

  VRT = analogRead(A2); //Acquisition analog value of VRT 

  VRT = (5.00 / 1023.00) * VRT; //Conversion to voltage 

  VR = VCC - VRT; 

  RT = VRT / (VR / R); //Resistance of RT 

  ln = log(RT / RT0); 

  TX = (1 / ((ln / B) + (1 / T0))); //Temperature from thermistor 

  TX = TX - 273.15; //Conversion to Celsius 
  
  uint8_t tempBytes[sizeof(double)];
  memcpy(tempBytes, &TX, sizeof(double));

  Serial.print(F("Updating Temperature value to "));
  Serial.print(TX);
  Serial.println(F(" Celsius"));


  gatt.setChar(tempMeasureCharId, tempBytes, 8);
  gatt.getChar(tempMeasureCharId);
  

  //Solar values

  VOUT1 = analogRead(A0); 

  VOUT1 = (5.00 / 1023.00) * VOUT1;  

  VIN1 = ((VOUT1 *(4720+980) / 980)); //Voltage Divider 

  uint8_t solBytes[sizeof(double)];
  memcpy(solBytes, &VIN1, sizeof(double));

  Serial.print(F("Updating Solar value to "));
  Serial.print(VIN1);
  Serial.println(F(" Volts"));

  gatt.setChar(solMeasureCharId, solBytes, 8);
  gatt.getChar(solMeasureCharId);

  //Battery values

  VOUT0 = analogRead(A1); 

  VOUT0 = (5.00 / 1023.00) * VOUT0;  

  VIN0 = ((VOUT0 *(4710+980) / 980)); //Voltage Divider 

  uint8_t battBytes[sizeof(double)];
  memcpy(battBytes, &VIN0, sizeof(double));

  Serial.print(F("Updating Battery value to "));
  Serial.print(VIN0);
  Serial.println(F(" Volts"));

  gatt.setChar(battMeasureCharId, battBytes, 8);
  gatt.getChar(battMeasureCharId);

  /* Delay before next measurement update */
  delay(1000);
}
