# LightBox
A personal Arduino / ESP32 Platform.io project for controlling lights on an rc drift car based on receiver values. Could easily be adapted to suit any surface vehicle.
The controller code contained in src/main.cpp is intended to be very readable and self-documenting. Feel free to use/modify or create a pull request/ form if you want to use this elsewhere.

This project is designed to run on an Arduino device connected to a string of WS2811/12 LEDS from a single pin. Lamps are defined by their address in that led chain as well as their purpose (Always on, responds to signal, etc). The project also uses up to 4 serial ports on the Arduino device to read incoming receiver values. Typically you'd connect the receiver to serial pots using a Y adapter off of each servo signal wire (throttle, turn, aux). An enterprising person could change this to read values from a composite signal stream (S.Bus, I.Bus, PPM)

Some features: 

- Running lights
- Brake lights
- Turn Signals
- Reverse signal
- Throttle based exhaust backfire effect. 
- Configurable light colors
- Configurable light arrangement
- Global car state and receiver state objects
- Configurable car state logic
- Uses FastLED for LED control
- Uses ArduinoTimer for blinker intervals


## TODO

- Mode switching based on Aux channel
- Additional 'Light show' modes. (using WS2812FX if feasible). 

## Wish List


- Gyro based car states / effects.
- Engine sound simulator. Grain table synth?
