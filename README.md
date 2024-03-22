# Plant watering station

### Description
Project of plant watering station that works autonomously as well as manually, based on ESP32 dev board.
When turned on soil moisture sensors keep track of ammount of water in plant pot and water sensor keeps track of ammount of water in water container.
If soil moisture reading on both sensors is low, and water sensor reading didn't cross minimum value, pump will turn on for PUMP_DURATION ms.
After pumping, another automatic watering will not occure for the next REST_PERIOD ms, to ensure water has enough time to disperse across soil.
You can always manually pump water by pressing a button.


![IMG_20240317_135310](https://github.com/jmamej/Plant-Watering-Station/assets/57408600/386b5bdf-b5bf-4546-9411-abaf65300d42)


| Automatic watering    | Manual watering |
| --------------------- | --------------  |
| <img width="800" src="https://github.com/jmamej/Plant-Watering-Station/assets/57408600/ae927b15-a09f-4364-a02a-3a1ec77e89a8.gif">  | <img width="800" src="https://github.com/jmamej/Plant-Watering-Station/assets/57408600/375c5872-50bf-4d9b-89f2-866255500b19.gif">    |




## List of components

- ESP32 dev board

- 18650 Li-Ion with BMS (2x)

- 5 V step-up/ step-down voltage regulator (optional)

- 5 V water pump (with transparent water hose)

- Soil moisture sensor

- Water level sensor

- N-MOSFET/ PWM controller/ relay (for controlling water pump)

- SSD1306 OLED screen

- DTH11 temperature/ humidity sensor


![image](https://github.com/jmamej/Plant-Watering-Station/assets/57408600/e75ad2e3-2f3d-4d10-b3a3-d482caa0178e)

*list of components*


## Cricuit schematics

![image](https://github.com/jmamej/Plant-Watering-Station/assets/57408600/68fe4e5d-1de6-4ba1-b5fe-b76ea5e8fa47)


*circuit schematics made with EasyEDA*


# Changes during testing

### Poor sensor performance when powered continuously

Fix: Connection VCC pins of all 3 sensors to GPIO's of ESP32 for powering. Max GPIO output current ~ 20 mA. Sensors current consumption < 20 mA.
Sensors are being powered up, analog reading is being performed, sensors are being powered down. GPIO's are being used as power supplies for short bursts.

```
void sensorsPower(int p1, int p2, int p3)
{
  digitalWrite(WATER_SENSOR_POWER_PIN, p1);
  digitalWrite(MOISTURE_1_POWER_PIN, p2);
  digitalWrite(MOISTURE_2_POWER_PIN, p3);
}

void sensorsRead()
{
  sensorsPower(1, 1, 1);
  water_level = readAnalogAverage(WATER_LEVEL_PIN);
  moisture1 = readAnalogAverage(MOISTURE_SENSOR_1_PIN);
  moisture2 = readAnalogAverage(MOISTURE_SENSOR_2_PIN);
  sensorsPower(0, 0, 0);
}
```

Result: Readings satisfactionary

| Front    | Back |
| --------------------- | --------------  |
| <img width="400" src="https://github.com/jmamej/Plant-Watering-Station/assets/57408600/113391b4-f441-4478-ac92-1a8e960f9b6c.png">  | <img width="400" src="https://github.com/jmamej/Plant-Watering-Station/assets/57408600/df2d4c90-8320-45e3-9343-cb2da9760cb4.png">    |


# Configuration Settings:

- WATER_LEVEL_THRESHOLD 700 - almost empty, 2000 full (readings highly vary depending on submerge time of the sensor)

- MOISTURE_THRESHOLD 400 - very dry, 2000 - wet

- PUMP_DURATION depends on watering hose length

- REST_PERIOD 1 min - short rest period


# Comments:
Efficiency of the entire project is dependant on pump/ watering hose assembly.
Soil sensors deteriorate over time, higher thresholds might be necessary.
It is not advised to connect pump directly to IO pin. Use N-MOSFET (logic level with low Rds)/ MOSFET driver like DRV8838/ PWM module or relay.


