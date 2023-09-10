# attiny85_temp_rgb
Goal is to lights up an RGB led according the the measured temperature using ATTiny85.
The whole project should fit in a bottle cap, so it will light the water inside.

Illustration:

![image](https://github.com/omeriko9/attiny85_temp_rgb/assets/5153984/182cc313-af74-4705-86bd-bfab3d0ed3df)

A very bad prototype:

![IMG_7588](https://github.com/omeriko9/attiny85_temp_rgb/assets/5153984/0baba867-896b-4b56-8bca-c7f0f44a1cbc)

![IMG_7591](https://github.com/omeriko9/attiny85_temp_rgb/assets/5153984/b708f1e0-60a1-461c-bf74-1f89598e79fa)

![image](https://github.com/omeriko9/attiny85_temp_rgb/assets/5153984/a0d8fbde-d156-431b-abeb-bb5dbc445f2b)


The main challenge in this project, aside the form factor, is that the ATTiny85 has only 2 native hardware PWM pins, but for a complete control of the color, you need 3.

Other than that it is just a nice decorative project to "artistically" show the measured temperature.

Addition to the project might include utilizing the last available I/O pin for capacitive button in order to put it in and out of sleep.

The intention is to use LIR2023 battery which is a coin size rechargable 3.6v battery, that should suffice to power up all components.

Required hardware:
- ATTiny85
- LIR2032
- DS18b20 Temperature sensor
- 4.7K Ohm Resistor

Layout of the wiring:

![image](https://github.com/omeriko9/attiny85_temp_rgb/assets/5153984/d878c242-1caa-4bff-b612-7e1b3fcd3553)



