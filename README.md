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

- N-MOSFET/ PWM controller/ relay (for controlling water pump)

- Power switch with 2 channels (NCP382HD05AAR2G)

- SSD1306 OLED screen

- AHT10 temperature/ humidity sensor


![image](https://github.com/jmamej/Plant-Watering-Station/assets/57408600/e75ad2e3-2f3d-4d10-b3a3-d482caa0178e)

*list of components*


## Cricuit schematics

![image](https://github.com/jmamej/Plant-Watering-Station/assets/57408600/d38cacac-6e01-4486-9f10-dd62814eae1e)

*circuit schematics made with EasyEDA*


# Changes during testing

Replaced second MOSFET with power switch for powering sensors and OLED screen. Pin 18 pulled HIGH turns on channel 1 (AHT10, OLED) pin 23 pulled HIGH turns on channel 2 (sensors).

Result: Lower current during sleep mode

Previous:

- ON - 88 mA
- SLEEP - 16 mA

Current:

- ON - 34 mA
- SLEEP - 16 mA

| Running    | Deep Sleep |
| --------------------- | --------------  |
| <img width="400" src="https://github.com/jmamej/Plant-Watering-Station/assets/57408600/7bc92a80-33d8-4d49-b59c-982b57d4e1dd.png">  | <img width="400" src="https://github.com/jmamej/Plant-Watering-Station/assets/57408600/f8f6573b-b8e4-4450-bb93-df8a961559fa.png">    |


# Configuration Settings:

- WATER_LEVEL_THRESHOLD 400 - almost empty, 2000 full (readings highly vary depending on submerge time of the sensor)

- MOISTURE_THRESHOLD 1000 - very dry, 2800 - wet

- PUMP_DURATION depends on watering hose length


# Comments:
Efficiency of the entire project is dependant on pump/ watering hose assembly.
Soil sensors deteriorate over time, higher thresholds might be necessary.
It is not advised to connect pump directly to IO pin. Use N-MOSFET (logic level with low Rds)/ MOSFET driver like DRV8838/ PWM module or relay.


# Previous schematics:

1. Base version

![image](https://github.com/jmamej/Plant-Watering-Station/assets/57408600/bbc8947e-5e31-4a38-b4ab-baeabd9ae772)

Problems: 
-  Very high current draw, due to continous sensor powering and continous CPU work (no deep sleep).
-  Usage of DHT11 - long power up and reading times.

2. Added sleep mode and fly-back diode

![image](https://github.com/jmamej/Plant-Watering-Station/assets/57408600/56dc8f18-52f6-4bc7-99b9-bd6c31cdd2a3)

Problems:
- High current draw while awake (>100 mA)
- High current draw while in deep sleep, due to sensors being continuosly connected to 3,3 V buss. Additionally these sensors work poorly when constantly powered (readings drop significantly)

3. Added second MOSFET to turn sensors off while in deep sleep (current draw reduced to 16 mA)

![image](https://github.com/jmamej/Plant-Watering-Station/assets/57408600/874ac463-1474-4b1a-acff-afdac0b4d1c9)

Problems:
- High current draw while awake ~ 90 mA. Sensors still unnecesarily powered for the entire duration of CPU awake state.
- MOSFET lowered voltage from 3,3 V to 2,8-2,9 V causing powering problems.

4. Replaced second MOSFET with 2-channel power switch NCP382HD05AAR2G. Sensors are being powered separately from AHT10 and OLED. This reduced power consumption while on to 34 mA. Sensors are being powered on for fraction of a second:

```
  power_sensors(1);  //turn sensors on
  sensorsRead();    //read from sensors
  power_sensors(0);  //turn sensors off
```

![image](https://github.com/jmamej/Plant-Watering-Station/assets/57408600/71e3e5c5-e851-417e-8578-e700031aaf35)



