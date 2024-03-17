/*
Description:
This is a code for plant soil watering station that works autonomously as well as manually based on ESP32 dev board.
When turned on soil moisture sensors keep track of ammount of water in plant pot and water sensor keeps track of ammount of water in water container.
If soil moisture reading on both sensors is low, and water sensor reading didn't cross minimum value, pump will turn on for PUMP_DURATION ms.
After pumping, another automatic watering will not occure for the next REST_PERIOD ms, to ensure water has enough time to disperse across soil.
You can always manually pump water by pressing a button.

Configuration Settings:
WATER_LEVEL_THRESHOLD 400 - wery dry, 2000 - wet
MOISTURE_THRESHOLD 700 - almost empty, 2000 full
PUMP_DURATION depends on watering hose length
REST_PERIOD 1 min - short rest period

Comments:
Efficiency of the entire project is dependant on pump/ watering hose assembly.
Soil sensors deteriorate over time, higher thresholds might be necessary.
It is not advised to connect pump directly to IO pin. Use N-MOSFET (logic level with low Rds)/ MOSFET driver like DRV8838/ PWM module or relay.


Author: JMamej

Date: 11.03.2024

Version: 0.1
*/

#include "DHT.h"
#include "SSD1306Wire.h"

#define DHTPIN 32
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

SSD1306Wire display(0x3c, SDA, SCL);

#define WATER_LEVEL_PIN 13
#define MOISTURE_SENSOR_1_PIN 14
#define MOISTURE_SENSOR_2_PIN 34
#define PUMP_PIN 2
#define BUTTON_PIN 25
#define ERROR_LED 4

#define WATER_LEVEL_THRESHOLD 400
#define MOISTURE_MIN_THRESHOLD 700
#define MOISTURE_MAX_THRESHOLD 2000
#define MOISTURE_MAX_INCONSISTENCY 700
#define PUMP_DURATION 3000  // 3 seconds watering
#define REST_PERIOD 60000  // 1 minutes timeout

unsigned long screen_timeout, current_millis, previous_millis = 0;
bool pump_running = false;
bool button_state = false;
bool manual_pump = false;
bool last_button_state = false;
bool error = false;

int water_level, moisture1, moisture2;

String error_string;

// function declaration
void resolvePump();
int readAnalogAverage(int pin);
void sensorsRead();
float checkTemperature();
float checkHumidity();
float checkHeatIndex();
void checkErrors();
void setScreenHeader();
void setScreenBody();
void updateScreen();

void setup(){
  Serial.begin(115200);
  dht.begin();
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  pinMode(WATER_LEVEL_PIN, INPUT);
  pinMode(MOISTURE_SENSOR_1_PIN, INPUT);
  pinMode(MOISTURE_SENSOR_2_PIN, INPUT);
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(ERROR_LED, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLDOWN); // Set button pin as input with internal pull-down resistor  (INPUT_PULLDOWN)
  sensorsRead();
}

void loop(){
  current_millis = millis();
  resolvePump();
  updateScreen();
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
  int button_state = digitalRead(BUTTON_PIN);
  // Check sensors
  sensorsRead();
  // Check if button is pressed
  if (button_state == HIGH && water_level > WATER_LEVEL_THRESHOLD) {
    digitalWrite(PUMP_PIN, HIGH);  // Turn on pump
    previous_millis = current_millis; // Update previousMillis when button is pressed
    manual_pump = true;
  }
  
  // Check if button is released and manual pump is on
  if (button_state == LOW && manual_pump) {
    digitalWrite(PUMP_PIN, LOW); // Turn off pump only if manual pump was on
    manual_pump = false;
  }

  // Check if it's time to stop the pump
  if (water_level < WATER_LEVEL_THRESHOLD || pump_running && current_millis > previous_millis + PUMP_DURATION){
    digitalWrite(PUMP_PIN, LOW);  // Turn off pump if it was running for the duration (PUMP_DURATION)
    pump_running = false;
  }
  
  // Check if it's time to run the pump
  if (!pump_running && current_millis > previous_millis + REST_PERIOD){

    // Check if all conditions are met
    if (water_level > WATER_LEVEL_THRESHOLD && moisture1 < MOISTURE_MIN_THRESHOLD && moisture2 < MOISTURE_MIN_THRESHOLD){
      digitalWrite(PUMP_PIN, HIGH);  // Turn on pump
      pump_running = true;
      previous_millis = current_millis;
    }
  }
}

// Calculate and return average of 8 readings (low ADC accuracy)
int readAnalogAverage(int pin)
{
  int result = 0;
  for (int i = 0; i < 8; i++){
    result += analogRead(pin);
  }
  return result / 8;
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
  return  dht.readTemperature();
}

// Read humidity from DHT
float checkHumidity()
{
  return dht.readHumidity();
}

// Read heat index from DHT
float checkHeatIndex()
{
  return (dht.computeHeatIndex(checkTemperature(), checkHumidity(), false));
}

// Check errors
void checkErrors()
{
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
    error_string = "t: " + String(checkTemperature(), 1) + " °C h: " + String(checkHumidity(), 0) + " %";
    error = false;
  }
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
  error ? digitalWrite(ERROR_LED, HIGH) : digitalWrite(ERROR_LED, LOW);
  if(current_millis > screen_timeout)
  {
    display.clear();
    setScreenHeader();
    setScreenBody();
    display.display();
    screen_timeout = current_millis + 500;
  }
}
