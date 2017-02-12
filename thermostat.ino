#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>
#include "LCD.h"
#include "menu.h"

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 12
#define TEMPERATURE_PRECISION 12

// Uncomment this line if you are using the updated dallas_temp_library that
// supports the busFail() method to diagnose bus problems
#define BUSFAIL
#define EEPROM_START_TEMP_ADDRESS   0
#define EEPROM_STOP_TEMP_ADDRESS    1
#define EEPROM_MODE_ADDRESS         2

#define JOYSTICK_VR_Y               0
#define JOYSTICK_VR_X               1
#define RELAY_PIN                   9

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

int numberOfDevices; // Number of temperature devices found

DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address

// diode status - on load low
int error_diode_on = LOW;
LCDMenu menu;
bool joyLock = false;
unsigned long lastTempRequest = 0;
unsigned long lastKeyPressed = 0;
int  delayInMillis = 1000;

void setup(void)
{

  // This lines just make it so you can plug a DS18B20 directly into
  // digitial pins 8-10.
  pinMode( PIN_DIODE_1  , OUTPUT );
  pinMode( RELAY_PIN  , OUTPUT );

  digitalWrite( PIN_DIODE_1 , LOW );
  digitalWrite( RELAY_PIN , HIGH );

  // start serial port
  Serial.begin(9600);

  loadConfigFromEEPROM();
  initalizeTermalSensor();

  startScreen();

}

void loadConfigFromEEPROM()
{
  int tmp = EEPROM.read(EEPROM_START_TEMP_ADDRESS);
  if (tmp == 255) {
    EEPROM.write(EEPROM_START_TEMP_ADDRESS, menu.start_temp);
    Serial.print("Writing EEPROM pump start temp");
  } else {
    menu.start_temp = tmp;
    Serial.print("EEPROM Pump start temp: ");
    Serial.print(menu.start_temp);
    Serial.println("");
  }
  tmp = EEPROM.read(EEPROM_STOP_TEMP_ADDRESS);
  if (tmp == 255) {
    EEPROM.write(EEPROM_STOP_TEMP_ADDRESS, menu.stop_temp);
    Serial.print("Writing EEPROM pump stop temp");
  } else {
    menu.stop_temp = tmp;
    Serial.print("EEPROM Pump stop temp: ");
    Serial.print(menu.stop_temp);
    Serial.println("");
  }
  menu.mode = EEPROM.read(EEPROM_MODE_ADDRESS);

}

void saveConfigToEEPROM(LCDMenu *menu)
{
  EEPROM.write(EEPROM_START_TEMP_ADDRESS, menu->start_temp);
  EEPROM.write(EEPROM_STOP_TEMP_ADDRESS, menu->stop_temp);
  EEPROM.write(EEPROM_MODE_ADDRESS, menu->mode);

}

int initalizeTermalSensor() {
  sensors.setWaitForConversion(false);
  sensors.begin();

  numberOfDevices = sensors.getDeviceCount();
  for (int i = 0; i < numberOfDevices; i++)
  {
    // Search the wire for address
    if (sensors.getAddress(tempDeviceAddress, i))
    {
      sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
    }
  }
  sensors.requestTemperatures();
  lastTempRequest = millis();
  //  sensors.reset();
  return numberOfDevices;
}

void loop(void)
{
  //chekc if some sensform found
  if (numberOfDevices == 0) {
    //no sersors found initailize and check again
    if (initalizeTermalSensor() == 0) {
      //print to serial
      Serial.print("Error: No sensros found!!!");
      //and blink diode
      error_diode_on = error_diode_on ? LOW : HIGH;
      digitalWrite( PIN_DIODE_1 , error_diode_on );

      LcdClear();
      LcdSetCursor(0, 2);
      LcdString("=Sensor Error=", error_diode_on);
      LcdSetCursor(0, 0);
      delay(250);
    } else {
      // found new sensor so turn off diode
      digitalWrite( PIN_DIODE_1 , LOW );
    }
  } else {
    //have some sensors so send the command to get temperatures
    // Search the wire for address
    Serial.print("Analog_0: ");
    int Ypos = analogRead(JOYSTICK_VR_Y);
    int Xpos = analogRead(JOYSTICK_VR_X);
    if (Ypos <= 100 && joyLock == false) {
      joyLock = true;
      menu.menuPos--;
      lastKeyPressed = millis();
    } else if (Ypos >= 1024 - 100 && joyLock == false) {
      joyLock = true;
      menu.menuPos++;
      lastKeyPressed = millis();
    } else if (Xpos <= 100 && joyLock == false) {
      joyLock = true;
      menuChangeItem(&menu, 1);
      saveConfigToEEPROM(&menu);
      lastKeyPressed = millis();
    } else if (Xpos >= 1024 - 100 && joyLock == false) {
      joyLock = true;
      menuChangeItem(&menu, -1);
      saveConfigToEEPROM(&menu);
      lastKeyPressed = millis();
    } else if (
      (Ypos > 100 && Ypos < 1024 - 100 && Xpos > 100 && Xpos < 1024 - 100) ||
      (millis() - lastKeyPressed >= 250)
    ) {
      joyLock = false;
    }

    if (millis() - lastKeyPressed >= 20000) {
      digitalWrite( PIN_LCD , LOW);
    } else {
      digitalWrite( PIN_LCD , HIGH);
    }

    Serial.println(Ypos);

    if (menu.mode == 0 ) {
      digitalWrite( PIN_DIODE_1 , LOW );
      digitalWrite( RELAY_PIN , HIGH );
      menu.pump_status = false;
    }  else  if (menu.mode == 1) {
      digitalWrite( PIN_DIODE_1 , HIGH );
      digitalWrite( RELAY_PIN , LOW );
      menu.pump_status = true;
    } else {
      menu.mode = 2;
      if (menu.temp >= menu.start_temp)  {
        digitalWrite( PIN_DIODE_1 , HIGH );
        digitalWrite( RELAY_PIN , LOW );
        menu.pump_status = true;
      }

      if (menu.temp < menu.stop_temp)  {
        digitalWrite( PIN_DIODE_1 , LOW );
        digitalWrite( RELAY_PIN , HIGH );
        menu.pump_status = false;
      }
    }

    drawMenu(&menu);
    if (millis() - lastTempRequest >= delayInMillis) // waited long enough??
    {
      // Output the device ID to serial
      Serial.print("Temp: ");
      //get temp in celcius
      if (sensors.getAddress(tempDeviceAddress, numberOfDevices - 1)) {
        menu.temp = sensors.getTempC(tempDeviceAddress);
        sensors.requestTemperatures();
        lastTempRequest = millis();

        //send to serial
        Serial.print(menu.temp, 1);
        Serial.println("*C");
      } else {
        initalizeTermalSensor();
      }
    }
  }

}


