# LightBox
Arduino / ESP32 project for controlling lights on an rc drift car based on receiver values. Could easily be adapted to suit any surface vehicle.
The controller code contained in src/main.cpp is intended to be very readable and self-documenting.

Some features: 

- Running lights
- Brake lights
- Turn Signals
- Reverse signal
- Throttle based exhaust blue flame effect. 
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


- Gyro channel passthrough for rule based gain adjustments (eg. if lateral g's indicate likely spinout, increase gain). The purpose of this is to limit gyro gain during low inertia moments, making it easier to initiate a slide, increasing gain as the car starts to approach lateral movement indicating potential spinout.
- Gyro based car states / effects
- Engine sound simulation. I believe this should be possible on ESP32, but will need to evaluate if any refactoring of blocking code is neccesary
