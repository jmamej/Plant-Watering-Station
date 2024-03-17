# Plant watering station

### Description
Project of plant watering station that works autonomously as well as manually, based on ESP32 dev board.
When turned on soil moisture sensors keep track of ammount of water in plant pot and water sensor keeps track of ammount of water in water container.
If soil moisture reading on both sensors is low, and water sensor reading didn't cross minimum value, pump will turn on for PUMP_DURATION ms.
After pumping, another automatic watering will not occure for the next REST_PERIOD ms, to ensure water has enough time to disperse across soil.
You can always manually pump water by pressing a button.


![IMG_20240317_135310](https://github.com/jmamej/Plant-Watering-Station/assets/57408600/386b5bdf-b5bf-4546-9411-abaf65300d42)


| Automatic watering    | Manual watering |
| --------------------------------------------- | ---------------------------------------------  |
| <img width="900" src="https://github.com/jmamej/Plant-Watering-Station/assets/57408600/ae927b15-a09f-4364-a02a-3a1ec77e89a8.gif">  | <img width="900" src="https://github.com/jmamej/Plant-Watering-Station/assets/57408600/375c5872-50bf-4d9b-89f2-866255500b19.gif">    |




## List of components

- ESP32 dev board

- 18650 Li-Ion with BMS

- 5 V step-up voltage regulator (optional)

- 5 V water pump (with transparent water hose)

- Soil moisture sensor

- Water level sensor

- N-MOSFET/ PWM controller/ relay (for controlling water pump)

- SSD1306 OLED screen

- DTH11 temperature/ humidity sensor


![image](https://github.com/jmamej/Plant-Watering-Station/assets/57408600/e75ad2e3-2f3d-4d10-b3a3-d482caa0178e)

*list of components*


## Cricuit schematics

![image](https://github.com/jmamej/Plant-Watering-Station/assets/57408600/f8ca484a-2b21-4ae6-bb3d-7d026dfb0d8e)

*circuit schematics made with EasyEDA*


# Configuration Settings:

- WATER_LEVEL_THRESHOLD 400 - very dry, 2000 - wet

- MOISTURE_THRESHOLD 700 - almost empty, 2000 full

- PUMP_DURATION depends on watering hose length

- REST_PERIOD 1 min - short rest period


# Comments:
Efficiency of the entire project is dependant on pump/ watering hose assembly.
Soil sensors deteriorate over time, higher thresholds might be necessary.
It is not advised to connect pump directly to IO pin. Use N-MOSFET (logic level with low Rds)/ MOSFET driver like DRV8838/ PWM module or relay.


