# attiny85_temp_rgb
Goal is to lights up an RGB led according the the measured temperature using ATTiny85.
The whole project should fit in a bottle cap, so it will light the water inside.
The colors transitions are smooth between each color.

![Bottle Cap Temperature RGB](https://github.com/omeriko9/attiny85_temp_rgb/assets/5153984/1165fdb6-0bf3-4a1a-85f0-aaf182df7579)

## Color Table:

| Temperature in °C | Color Name     | Color Hex |
--------------------------------------------------
| <=17              | Deep Blue      | 0x0000FF  |
| 18 - 20           | Light Blue     | 0x00FFFF  |
| 21 - 22           | Cyan           | 0x00FFFF  |
| 23 - 24           | Green          | 0x00FF00  |
| 25                | Light Green    | 0x7FFF00  |
| 26                | Green-Orange   | 0x7FFF7F  |
| 27                | Orange         | 0xFF7F00  |
| 28                | Yellow         | 0xFFFF00  |
| 29                | Yellow-Red     | 0xFFFF7F  |
| 30-31             | Red            | 0xFF0000  |
| 32                | Red-Purple     | 0xFF007F  |
| >=33              | Purple         | 0x7F007F  |




## Illustration of the project's components:

![image](https://github.com/omeriko9/attiny85_temp_rgb/assets/5153984/182cc313-af74-4705-86bd-bfab3d0ed3df)

## V1 of the printed circuit board (gerber and BOM files are in the repository):

![image](https://github.com/omeriko9/attiny85_temp_rgb/assets/5153984/b7f3f476-aff5-4ec0-a92d-0ddf667e67dd)

![image](https://github.com/omeriko9/attiny85_temp_rgb/assets/5153984/ca66abef-b630-4d00-8b8a-805cfa5104ec)

![image](https://github.com/omeriko9/attiny85_temp_rgb/assets/5153984/54f986fc-ad30-448c-8804-c1e54d110b52)

## Motivation

The main challenge in this project, aside the form factor, is that the ATTiny85 has only 2 native hardware PWM pins, but for a complete control of the color, you need 3.
Other than that it is just a nice decorative project to "artistically" show the measured temperature.
Addition to the project might include utilizing the last available I/O pin for capacitive button in order to put it in and out of sleep.
The intention is to use LIR2023 battery which is a coin size rechargable 3.6v battery, that should suffice to power up all components.

## Required hardware

- ATTiny85
- LIR2032
- DS18b20 Temperature sensor
- 4.7K Ohm Resistor

## Circuit Diagram

![image](https://github.com/omeriko9/attiny85_temp_rgb/assets/5153984/aece5bb8-94d6-45c7-8286-b1138b543f22)




