# LightBox
Arduino / ESP32 project for controlling lights on an rc car based on rc receiver values. 
This code in src/main.cpp is intended to be very readable and self-documenting.

Some features: 

- Configurable light colors
- Configurable light arrangement
- Global car state and receiver state objects
- Configurable car state logic
- Uses FastLED for LED control
- Uses ArduinoTimer for blinker intervals


## TODO

- Mode switching based on Aux channel
- Additional 'Light show' modes. (using WS2812FX if feasible). 
