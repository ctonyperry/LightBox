#define FASTLED_INTERNAL //Supress FastLED pragma warning
#include <FastLED.h>
#include <ServoInput.h>
#include <HardwareSerial.h>
#include <arduino-timer.h>

#define LED_COUNT 17
#define LED_PIN 15

#define THROTTLE_RC_PIN 4
#define STEERING_RC_PIN 5

CRGB leds[LED_COUNT];

CHSV headlightColor = CHSV(70, 45, 128);
CHSV highbeamColor = CHSV(193, 115, 255);
CHSV turnSignalColor = CHSV(64, 255, 145);
CHSV fogLightColor = CHSV(70, 250, 255);
CHSV brakelightColor = CHSV(0, 255, 255);
CHSV brakelightDimColor = CHSV(0, 255, 55);
CHSV exhaustLampColor = CHSV(150, 255, 50);
CHSV offColor = CHSV(0, 0, 0);

enum RCChannels
{
  STEERING,
  THROTTLE,
  AUX
};

struct ReceiverStates
{
  int steeringValue;
  int throttleValue;
  int auxValue;
};

float steeringAngle;
int throttlePercent;

enum CarLamps
{
  LF_TURN,
  LF_HEADLIGHT,
  LF_HIGHBEAM,
  LF_FOG,
  RF_HIGHBEAM,
  RF_HEADLIGHT,
  RF_TURN,
  RF_FOG,
  RR_BRAKE,
  RR_TURN,
  LR_TURN,
  LR_BRAKE,
  C_RUNNING,
  RR_LICENSE_PLATE,
  LR_LICENSE_PLATE,
  EXHAUST_1,
  EXHAUST_2
};

struct LampConfig
{
  CarLamps carLamp;
  CHSV activeColor;
  CHSV inactiveColor;
};

LampConfig lampsConfig[] = {
    {LF_TURN, turnSignalColor, offColor},
    {LF_HEADLIGHT, headlightColor, offColor},
    {LF_HIGHBEAM, highbeamColor, offColor},
    {LF_FOG, fogLightColor, offColor},
    {RF_HIGHBEAM, highbeamColor, offColor},
    {RF_HEADLIGHT, headlightColor, offColor},
    {RF_TURN, turnSignalColor, offColor},
    {RF_FOG, fogLightColor, offColor},
    {RR_TURN, turnSignalColor, offColor},
    {RR_BRAKE, brakelightColor, brakelightDimColor},
    {LR_BRAKE, brakelightColor, brakelightDimColor},
    {LR_TURN, turnSignalColor, offColor},
    {C_RUNNING, brakelightColor, brakelightDimColor},
    {RR_LICENSE_PLATE, brakelightColor, offColor},
    {LR_LICENSE_PLATE, headlightColor, offColor},
    {EXHAUST_1, exhaustLampColor, offColor},
    {EXHAUST_2, exhaustLampColor, offColor}};

LampConfig getLampConfig(CarLamps carLamp)
{
  for (int i = 0; i < LED_COUNT; i++)
  {
    if (lampsConfig[i].carLamp == carLamp)
    {
      return lampsConfig[i];
    }
  }
};

enum CarStates
{
  DRIVING,
  TURBO,
  BRAKING,
  REVERSING,
  IDLING,
  TURNING_LEFT,
  TURNING_RIGHT,
  NOT_TURNING
};

CarStates carState[2];

const int SteeringSignalPin = STEERING_RC_PIN; // MUST be interrupt-capable!
const int SteeringPulseMin = 1000;             // microseconds (us)
const int SteeringPulseMax = 2000;             // Ideal values for your servo can be found with the "Calibration" example

ServoInputPin<SteeringSignalPin> steering(SteeringPulseMin, SteeringPulseMax);

// Throttle Setup
const int ThrottleSignalPin = THROTTLE_RC_PIN; // MUST be interrupt-capable!
const int ThrottlePulseMin = 1000;             // microseconds (us)
const int ThrottlePulseMax = 2000;             // Ideal values for your servo can be found with the "Calibration" example

ServoInputPin<ThrottleSignalPin> throttle(ThrottlePulseMin, ThrottlePulseMax);

const int blinkerIntervalMs = 1500;
auto timer = timer_create_default();

void updateSteeringState()
{
  if (steeringAngle < -30.0)
  {
    carState[STEERING] = TURNING_LEFT;
  }
  else if (steeringAngle > 30)
  {
    carState[STEERING] = TURNING_RIGHT;
  }
  else
  {
    carState[STEERING] = NOT_TURNING;
  }
};

void updateThrottleState()
{
  if (throttlePercent < -75)
  {
    carState[THROTTLE] = REVERSING;
  }
  else if (throttlePercent < -5)
  {
    carState[THROTTLE] = BRAKING;
  }
  else if (throttlePercent <= 10)
  {
    carState[THROTTLE] = DRIVING;
  }
  else if (throttlePercent >= 33)
  {
    carState[THROTTLE] = TURBO;
  }
  else
  {
    carState[THROTTLE] = IDLING;
  }
};

void updateCarStates()
{
  updateSteeringState();
  updateThrottleState();
};

void updateRCValues()
{
  steeringAngle = (90.0 - steering.getAngle()) * -1; // returns 0 - 180, subtracting from 90 to center at "0" and invert for "normal" steering
  throttlePercent = throttle.map(-100, 100);         // remap to a percentage both forward and reverse
};


bool blinkerOn = false;

bool serviceSteeringSignalLamps(void *)
{
  CHSV toggleColor;
  if(!blinkerOn){
    toggleColor = turnSignalColor;
    blinkerOn = true;
  } else {
    toggleColor = offColor;
    blinkerOn = false;
  }
switch(carState[STEERING]){
  case TURNING_LEFT:
    leds[LF_TURN] = toggleColor;
    leds[LR_TURN] = toggleColor;
    leds[RF_TURN] = offColor;
    leds[RR_TURN] = offColor;
  break;

  case TURNING_RIGHT:
    leds[RF_TURN] = toggleColor;
    leds[RR_TURN] = toggleColor;
    leds[LF_TURN] = offColor;
    leds[LR_TURN] = offColor;
  break;

  default:
    leds[LF_TURN] = offColor;
    leds[LR_TURN] = offColor;
    leds[RF_TURN] = offColor;
    leds[RR_TURN] = offColor;
  break;
};
return true;
}



void initRunningMode()
{
  //Default light states for standard running mode
  leds[LF_HEADLIGHT] = getLampConfig(LF_HEADLIGHT).activeColor;
  leds[RF_HEADLIGHT] = getLampConfig(RF_HEADLIGHT).activeColor;
  leds[LR_BRAKE] = getLampConfig(LR_BRAKE).inactiveColor;
  leds[RR_BRAKE] = getLampConfig(RR_BRAKE).inactiveColor;
  leds[LR_LICENSE_PLATE] = getLampConfig(LR_LICENSE_PLATE).inactiveColor;
  leds[RR_LICENSE_PLATE] = getLampConfig(LR_LICENSE_PLATE).inactiveColor;
  leds[C_RUNNING] = getLampConfig(C_RUNNING).inactiveColor;
  leds[EXHAUST_1] = getLampConfig(EXHAUST_1).inactiveColor;
  leds[EXHAUST_2] = getLampConfig(EXHAUST_2).inactiveColor;
}



void serviceTurboMode(){

int randomValue = random(0, throttlePercent * exhaustLampColor.val);
CHSV tempColor = CHSV(exhaustLampColor.hue, exhaustLampColor.sat - (exhaustLampColor.sat * throttlePercent), randomValue);
  leds[EXHAUST_1] = tempColor;
  leds[EXHAUST_2] = tempColor;
  Serial.println(randomValue);
}

void serviceThrottleSignalLamps()
{
  switch (carState[THROTTLE])
  {
  case DRIVING:
    //move driving mode init here from main loop. 
    break;

  case TURBO:
  serviceTurboMode();
  break;

  case BRAKING:
    leds[RR_BRAKE] = getLampConfig(RR_BRAKE).activeColor;
    leds[LR_BRAKE] = getLampConfig(LR_BRAKE).activeColor;
    leds[C_RUNNING] = getLampConfig(C_RUNNING).activeColor;
    break;
  case REVERSING:
    leds[LR_LICENSE_PLATE] = getLampConfig(LR_LICENSE_PLATE).activeColor;
    leds[RR_LICENSE_PLATE] = getLampConfig(RR_LICENSE_PLATE).activeColor;
    break;
  case IDLING:
    //todo countdown to lights off
    break;
  }
};

void serviceSignalLamps()
{
  initRunningMode();
  serviceThrottleSignalLamps();
};

void setup()
{
  
  Serial.begin(9600);
  FastLED.addLeds<WS2812B, LED_PIN, RGB>(leds, LED_COUNT);
  while (!ServoInput.available())
  { // wait for all signals to be ready
    //Serial.println("Waiting for servo signals...");
    delay(500);
  }
 timer.every(1000, serviceSteeringSignalLamps);
 
};

void loop()
{
  updateRCValues();
  updateCarStates();
  serviceSignalLamps();
  timer.tick();
  FastLED.show();
}
