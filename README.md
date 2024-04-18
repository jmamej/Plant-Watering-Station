# Plant watering station

### Description
Project of plant watering station that works autonomously as well as manually, based on ESP32 dev board.
Sleep mode intorduced to reduce power consumption. When turned on soil moisture sensors keep track of ammount of water in plant pot and water sensor keeps track of ammount of water in water container.
If soil moisture reading on both sensors is low, and water sensor reading didn't cross minimum value, pump will turn on for PUMP_DURATION ms (manual watering is also available), after which esp32 goes to deep sleep. Wake up is initiated either with timer or button press.


![IMG_20240322_150134](https://github.com/jmamej/Plant-Watering-Station/assets/57408600/d6268ea5-380d-4d0e-9479-58da871564b0)



| Automatic watering    | Manual watering |
| --------------------- | --------------  |
| <img width="800" src="https://github.com/jmamej/Plant-Watering-Station/assets/57408600/ae927b15-a09f-4364-a02a-3a1ec77e89a8.gif">  | <img width="800" src="https://github.com/jmamej/Plant-Watering-Station/assets/57408600/375c5872-50bf-4d9b-89f2-866255500b19.gif">    |




## List of components

- ESP32 dev board

- 18650 Li-Ion with BMS

- 5 V step-up voltage regulator

- 5 V water pump (with transparent water hose)

- 2x Soil moisture sensor

- Water level sensor

- 2x N-MOSFET/ PWM controller/ relay (for controlling water pump)

- SSD1306 OLED screen

- AHT10 temperature/ humidity sensor


![image](https://github.com/jmamej/Plant-Watering-Station/assets/57408600/e75ad2e3-2f3d-4d10-b3a3-d482caa0178e)

*list of components*


## Cricuit schematics

![image](https://github.com/jmamej/Plant-Watering-Station/assets/57408600/bc9cbf92-9858-4f9d-af2c-7ba627c32c02)


*circuit schematics made with EasyEDA*


# Changes during testing

Added second MOSFET for powering sensors and OLED screen. Pin 18 pulled HIGH turns on MOSFET based switch, enabling current flow to sensors.

Result: Lower current during sleep mode

- ON - 88 mA
- SLEEP - 16 mA

| Running    | Deep Sleep |
| --------------------- | --------------  |
| <img width="400" src="https://github.com/jmamej/Plant-Watering-Station/assets/57408600/18095739-64d6-4d58-9485-415a0b317a65.png">  | <img width="400" src="https://github.com/jmamej/Plant-Watering-Station/assets/57408600/4721bc3c-9656-456c-83ad-b43ab9002c7d.png">    |


# Configuration Settings:

- WATER_LEVEL_THRESHOLD 400 - almost empty, 2000 full (readings highly vary depending on submerge time of the sensor)

- MOISTURE_THRESHOLD 1000 - very dry, 2800 - wet

- PUMP_DURATION depends on watering hose length


# Comments:
Efficiency of the entire project is dependant on pump/ watering hose assembly.
Soil sensors deteriorate over time, higher thresholds might be necessary.
It is not advised to connect pump directly to IO pin. Use N-MOSFET (logic level with low Rds)/ MOSFET driver like DRV8838/ PWM module or relay.


