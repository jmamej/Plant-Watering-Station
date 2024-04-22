/*
Description:
This is a code for plant soil watering station that works autonomously as well as manually, based on ESP32 dev board. Sleep mode intorduced to reduce power consumption.
When turned on soil moisture sensors keep track of ammount of water in plant pot and water sensor keeps track of ammount of water in water container.
If soil moisture reading on both sensors is low, and water sensor reading didn't cross minimum value, pump will turn on for PUMP_DURATION ms (manual watering is also available), after which esp32 goes to deep sleep.
Wake up is initiated either with timer or button press.


Configuration Settings:
WATER_LEVEL_THRESHOLD 400 - almost empty, 2000 full
MOISTURE_THRESHOLD  1000 - wery dry, 2000 - wet
PUMP_DURATION depends on watering hose length

Comments:
Efficiency of the entire project is dependant on pump/ watering hose assembly.
Soil sensors deteriorate over time, higher thresholds might be necessary.
It is not advised to connect pump directly to IO pin. Use N-MOSFET (logic level with low Rds)/ MOSFET driver like DRV8838/ PWM module or relay.


Author: JMamej

Date: 22.04.2024

Version: 0.4
*/

#include <driver/rtc_io.h>
#include <Adafruit_AHT10.h>
#include "SSD1306Wire.h"

Adafruit_AHT10 aht;

sensors_event_t humidity, temp;

SSD1306Wire display(0x3c, SDA, SCL);

#define WATER_LEVEL_PIN 13
#define MOISTURE_SENSOR_1_PIN 14
#define MOISTURE_SENSOR_2_PIN 34
#define PUMP_PIN 2
#define BUTTON_PIN 4
#define ERROR_LED 19
#define POWER_PIN_OLED  18
#define POWER_PIN_SENSORS 23

#define WATER_LEVEL_THRESHOLD 400
#define MOISTURE_MIN_THRESHOLD 1000
#define MOISTURE_MAX_THRESHOLD 2800
#define MOISTURE_MAX_INCONSISTENCY 800
#define NUM_OF_SENSOR_READS 16
#define PUMP_DURATION 2000  // 2 seconds watering
#define MANUAL_WATER_READ_TIMER 100 // 100 ms

#define SCREEN_ON_SHORT 15000  //30000
#define SCREEN_ON_LONG  30000 //60000

#define SLEEP_TIME_ERROR      60000000  //300000000
#define SLEEP_TIME_NO_ERROR   300000000  //3600000000

unsigned long current_millis, previous_millis = 0;
bool pump_running = false;
int button_state = 0;
bool manual_pump = false;
bool error = false;

int water_level, moisture1, moisture2;

long long sleep_time, screen_on_time;

String error_string;

// function declaration
void power_3v3_buss(int on_off);
void resolvePump();
int readAnalogAverage(int pin);
void sensorsPower(int p1, int p2, int p3);
void waterSensorRead();
void sensorsRead();
float checkTemperature();
float checkHumidity();
void checkErrors();
void setScreenHeader();
void setScreenBody();
void updateScreen();
void sleep();


void setup(){
  Serial.begin(115200);
  pinMode(POWER_PIN_OLED, OUTPUT);
  pinMode(POWER_PIN_SENSORS, OUTPUT);
  pinMode(WATER_LEVEL_PIN, INPUT);
  pinMode(MOISTURE_SENSOR_1_PIN, INPUT);
  pinMode(MOISTURE_SENSOR_2_PIN, INPUT);
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(ERROR_LED, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Set button pin as input with internal pull-down resistor  (INPUT_PULLDOWN)
  power_oled(1);
  delay(1); //give time for sensors to power up before initializing

  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);

  display.clear();
  display.drawString(40, 24, "Hello!");
  display.display();

  while(! aht.begin() && millis() < 3000)
  {
    digitalWrite(ERROR_LED, HIGH);
    delay(1);
  }

  digitalWrite(ERROR_LED, LOW);

  rtc_gpio_pullup_en((gpio_num_t)BUTTON_PIN); //enable pullup after CPU is put to deep sleep
  esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_PIN, LOW);  //enable wakeup with button press

  power_sensors(1);
  sensorsRead();
  power_sensors(0);

  updateScreen();
}

void loop(){
  current_millis = millis();
  button_state = digitalRead(BUTTON_PIN);

  resolvePump();

  if(current_millis > screen_on_time)
  {
    sleep();
  }
}


/*
  Power up 3V3 power buss - turn sensors ond screen on
*/

void power_oled(int on_off)
{
  digitalWrite(POWER_PIN_OLED, on_off);
}

void power_sensors(int on_off)
{
  digitalWrite(POWER_PIN_SENSORS, on_off);
}


/*
* resolvePump runs pump manualy and automaticaly
* manual - pump runs if button is pressed
* automatic - water sensor is checked to ensure there is enough water (prevents pump running dry)
*           - moisture sensors check soil moisture levels
*           if assumptions are met pump will run for 5 seconds, then wait 5 minutes and check moisture level
*           this timeout of 5 minutes prevents soil being ovar watered
*           manual watering resets timeout, so soil is not watered automatically, moments after manual watering
*/
void resolvePump()
{
  // Check if button is pressed
  if (button_state == LOW && water_level > WATER_LEVEL_THRESHOLD) {

    digitalWrite(PUMP_PIN, HIGH);  // Turn on pump
    manual_pump = true;
  }
  
  // Check if button is released and manual pump is on
  if (button_state == HIGH && manual_pump) {
    digitalWrite(PUMP_PIN, LOW); // Turn off pump only if manual pump was on
    manual_pump = false;
  }

  // Check if it's time to stop the pump
  if (water_level < WATER_LEVEL_THRESHOLD || pump_running && current_millis > previous_millis + PUMP_DURATION){
    digitalWrite(PUMP_PIN, LOW);  // Turn off pump if it was running for the duration (PUMP_DURATION)
    pump_running = false;
  }
  
  // Check if all conditions are met
  if (water_level > WATER_LEVEL_THRESHOLD && !pump_running && moisture1 < MOISTURE_MIN_THRESHOLD && moisture2 < MOISTURE_MIN_THRESHOLD){
    digitalWrite(PUMP_PIN, HIGH);  // Turn on pump
    pump_running = true;
    previous_millis = current_millis;
  }
}

// Calculate and return average of 8 readings (low ADC accuracy)
int readAnalogAverage(int pin)
{
  int result = 0;
  for (int i = 0; i < NUM_OF_SENSOR_READS; i++){
    result += analogRead(pin);
  }
  return result / NUM_OF_SENSOR_READS;
}

// Read water level sensors
void waterSensorRead()
{
  water_level = readAnalogAverage(WATER_LEVEL_PIN);
}

// Read soil moisture and water level sensors
void sensorsRead()
{
  water_level = readAnalogAverage(WATER_LEVEL_PIN);
  moisture1 = readAnalogAverage(MOISTURE_SENSOR_1_PIN);
  moisture2 = readAnalogAverage(MOISTURE_SENSOR_2_PIN);
}

// Read temperature from DHT
float checkTemperature()
{
  return  (int)temp.temperature;
}

// Read humidity from DHT
float checkHumidity()
{
  return  (int)humidity.relative_humidity;
}

// Check errors
void checkErrors()
{
  screen_on_time = SCREEN_ON_LONG;  //leave screen for 60s if encountered errors
  if (water_level < WATER_LEVEL_THRESHOLD) {
    error_string = "Low water level";
    error = true;
  } else if (moisture1 > MOISTURE_MAX_THRESHOLD || moisture2 > MOISTURE_MAX_THRESHOLD) {
    error_string = "High moisture read";
    error = true;
  } else if (abs(moisture1 - moisture2) > MOISTURE_MAX_INCONSISTENCY) {
    error_string = "Inconsistent moisture";
    error = true;
  } else  {
    aht.getEvent(&humidity, &temp); // populate temp and humidity objects with fresh data
    error_string = "t: " + String(checkTemperature(), 1) + " °C h: " + String(checkHumidity(), 0) + " %";
    error = false;
    screen_on_time = SCREEN_ON_SHORT; //leave screen for 30s if no errors
  }
  error ? digitalWrite(ERROR_LED, HIGH) : digitalWrite(ERROR_LED, LOW);
}

// Set screen header only
void setScreenHeader()
{
  display.drawString(0, 0, error_string);
}

// Set screen body only
void setScreenBody()
{
  display.drawString(0, 16, "W: " + String(water_level));
  display.drawString(0, 32, "S1:  " + String(moisture1));
  display.drawString(0, 48, "S2: " + String(moisture2));
}

// Clear, update buffer and print to screen
void updateScreen()
{
  checkErrors();
  display.clear();
  setScreenHeader();
  setScreenBody();
  display.display();
}

//Reset sensors and initialize sleep;
void sleep()
{
  error ? sleep_time = SLEEP_TIME_ERROR : sleep_time = SLEEP_TIME_NO_ERROR;   //SLEEP_TIME_ERROR, SLEEP_TIME_NO_ERROR
  display.resetDisplay();
  power_oled(0);
  esp_deep_sleep(sleep_time);
}

