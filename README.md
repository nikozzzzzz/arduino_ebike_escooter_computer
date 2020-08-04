# arduino_ebike_escooter_computer
Using Arduino Uno to check battery voltage (7 cells, but you can do more), battery temperature, display it to 16x2 LCD, show alarms and beep to speaker on errors (like undervoltage, overvoltage, high temp, etc)

# You will need
1. Arduino UNO or compatible
2. 16x2 LCD with I2C board
3. 74HC4067 multiplexer board (or you can buy the IC and solder it all by hand)
4. 18B20 digital temperature sensor
5. 5 to 10 kOhm resistor (for the sensor)
6. A lot of 1% or better precision resistors to use as voltage dividers
7. Breadboard for easy prototyping
8. Soldering iron, flux, solder, skills
9. Digital or analog multimeter

If you dont plan to read temperature, just comment all code regarding 18B20 sensor.
If you plan to measure more than 7 battery cells, you will need more wires, more precision resistors. 74HC4067 has 16 inputs, so obviously you cant have more than 16 batteries measured by one IC. But you can connect more ICs and add code :)


# Setup
1. Different LCDs have different working drivers and addresses
2. Google your LCD, addreses and drivers they use.
3. Import your driver to the project. In my case it was hd44780.h, if yours is the same, leave it the way it is.
4. Connect all the stuff together

# Connections: Arduino Pins
Digital 0: empty
Digital 1: empty
Digital 2: temperature sensor middle leg
Digital 3: 100 Ohm resistor (other resistor legs goes to speaker +, other speaker leg goes to ground)
Digital 4: Mupliplexer S3 pin
Digital 5: Multiplexer S2 pin
Digital 6: Multiplexer S1 pin
Digital 7: Multiplexer S0 pin
Digital 8: empty
Digital 9: empty
Digital 10: empty
Digital 11: empty
Digital 12: empty
Digital 13: empty
SCL: display SCL
SDA: display SDA
Analog 0: Multiplexer SIG
Analog 1: empty
Analog 2: empty
Analog 3: empty
Analog 4: empty
Analog 5: empty

Temp sensor: connect a 5-10K resistor from middle leg to right leg. Connect right leg to +5V, left leg to GND

Everything is powered from 5v: Arduino, Display, Multiplexer, Temp sensor.

# Troubleshooting
No image on screen - use the potentiometer on back of the screen to adjust contrast
No temp data - check sensor connection, use a 5K resistor
Phantom readings on analog pins - its ok if you have them hanging free in air or nothing connected.

# Questions, feedback
If you have any questions and feedback, feel free to contact me nikozzzzzz@gmail.com or telegram at @nikozzzzzz








