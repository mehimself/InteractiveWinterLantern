#include <Adafruit_NeoPixel.h>

#define LEDPIN 8
#define LEDCOUNT 22

#define LIGHTSENSORPIN A0

#define STAR 3
#define TV 1
#define TREE 2
#define SANTA 12

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEDCOUNT, LEDPIN);

byte cloud[] = {4, 5, 6};
byte house[] = {0, 1};
byte ambient[] = {7, 8, 9, 10, 11, 13, 16, 17, 18, 19, 20, 21};
byte ambientLeft1 = 7;
byte ambientLeft2 = 8;
byte ambientLeft3 = 9;
byte ambientLeft4 = 10;
byte ambientBottomLeft = 11;
byte ambientBottomRight = 17;
byte ambientRight4 = 18;
byte ambientRight3 = 19;
byte ambientRight2 = 20;
byte ambientRight1 = 21;
byte herd[] = {14, 15};

uint32_t glare = strip.Color(255, 255, 255);
uint32_t coolWhite = strip.Color(170, 180, 200);
uint32_t warmWhite = strip.Color(255, 160, 70);
uint32_t darkBlue = strip.Color(0, 10, 20);
uint32_t white = strip.Color(255, 255, 255);
uint32_t green = strip.Color(0, 180, 20);
uint32_t red = strip.Color(170, 10, 0);
uint32_t orange = strip.Color(255, 180, 40);
uint32_t brown = strip.Color(150, 104, 0);
uint32_t midnightBlue = strip.Color(0, 90, 150);

byte sensorCalibration = 127;
byte sensorVectorBalance = sensorCalibration;
byte sensorDampening = 3;
int brightnessDamping = 20;

bool lightningInProgress = false;
byte lightningDuration = 0;

float minBrightness = 5;
float maxBrightness = 255;
byte brightness = 2;

int period1 = 5100;
int period2 = 2050;
int period3 = 7800;

unsigned long time = millis();
unsigned long now = time;

unsigned long sinceLastSample = 0;
unsigned long lastLo = 0;
unsigned long lastHi = 0;
int gestureTimeout = 3000;
int gestureMinTime = 1000;

unsigned long animateNextDay = 0;

int dayDuration = 10000;
int hourDuration = dayDuration / 24;

float timeOfDay = 0;

bool animatingDay = false;
bool sunIsShining = false;
bool nightMode = true;
bool dayMode = false;

void animateReindeer() {
  byte herdSize = sizeof(herd);
  for (byte p = 0; p < herdSize; p++) {
    setPixel(herd[p], brown, brightness);
  }  
}
void resetAmbient() {
  for (byte i = 0; i < LEDCOUNT; i++) {
    if (i != TV) {
      strip.setPixelColor(i, 0);
    }
  }
}
byte queueNextDayAnimation() {
  if (!animatingDay) {
    unsigned long now = millis();
    unsigned long tillNextDay = dayDuration - (now % dayDuration);
    animateNextDay = now + tillNextDay;
  }
}
byte brightnessAscending(byte lo, byte hi, float val, byte peakIntensity) {
  byte intensity = 0;
  if (lo <= val && val < hi) {
    float range = (float(hi) - lo);
    float center = lo + range;
    float proportion = 1 - (float(hi) - val) / range;
    intensity = proportion * peakIntensity;
  }
  return intensity;
}
byte brightnessDescending(byte lo, byte hi, float val, byte peakIntensity) {
  byte intensity = 0;
  if (lo <= val && val < hi) {
    float range = (float(hi) - lo);
    float center = lo + range;
    float proportion = (float(hi) - val) / range;
    intensity = proportion * peakIntensity;
  }
  return intensity;
}
byte brightnessCurve(byte lo, byte hi, float val, byte peakIntensity) {
  byte intensity = brightness;
  if (lo <= val && val < hi) {
    float range = (float(hi) - lo) / 2;
    float center = lo + range;
    float proportion = abs((center - val) / range);
    float proximityToCenter = 1 - proportion;
    intensity = proximityToCenter * peakIntensity;
  }
  return intensity;
}
void animateTime() {
  now = millis();
  int dayTime = now % dayDuration;
  timeOfDay = float(dayTime) / hourDuration;  
  animatingDay = animateNextDay <= now && now < animateNextDay + dayDuration;
  sunIsShining = animatingDay && (7 <= timeOfDay || timeOfDay < 17);
}
void animateStar() {
  byte lo = 6;
  byte hi = 10;
  
  uint32_t color = coolWhite;

  bool fadeOut;
  bool fadeIn;
  
  if (random(30) < 1) {
    color = warmWhite;
  }

  lo = 6; hi = 10;
  fadeOut = animatingDay && lo <= timeOfDay && timeOfDay < hi;
  if (fadeOut) {
    setPixel(STAR, color, brightnessDescending(lo, hi, timeOfDay, brightness));
  }
  lo = 15; hi = 18;
  fadeIn = animatingDay && lo <= timeOfDay && timeOfDay < hi;
  if (fadeIn) {
    setPixel(STAR, color, brightnessAscending(lo, hi, timeOfDay, brightness));
  }
  setPixel(STAR, color, brightness);
}
void animateSunrise(byte maxIntensity) {
  byte lo = 6; 
  byte hi = 7;
  byte intensity = brightness;
  if (lo <= timeOfDay && timeOfDay < hi) {
    intensity = brightnessAscending(lo, hi, timeOfDay, min(maxIntensity, 128));
    setPixel(ambientRight4, red, intensity);
  }
  lo = 7; hi = 8;
  if (lo <= timeOfDay && timeOfDay < hi) {
    intensity = brightnessDescending(lo, hi, timeOfDay, maxIntensity);
    setPixel(ambientRight4, red, intensity);
  }
  lo = 7; hi = 9;
  if (lo <= timeOfDay && timeOfDay < hi) {
    intensity = brightnessAscending(lo, hi, timeOfDay, maxIntensity);
    setPixel(ambientRight3, orange, intensity);
  }
  
}
void animateMorning(byte maxIntensity) {
  byte lo = 9; 
  byte hi = 12;
  byte intensity = brightness;
  if (lo <= timeOfDay && timeOfDay < hi) {
    intensity = brightnessDescending(lo, hi, timeOfDay, maxIntensity);
    setPixel(ambientRight3, orange, intensity);
  }
  lo = 8; hi = 12;
  if (lo <= timeOfDay && timeOfDay < hi) {
    intensity = brightnessAscending(lo, hi, timeOfDay, maxIntensity);
    setPixel(ambientRight2, warmWhite, intensity);
  }
  lo = 8; hi = 13;
  if (lo <= timeOfDay && timeOfDay < hi) {
    intensity = brightnessAscending(lo, hi, timeOfDay, maxIntensity);
    setPixel(ambientRight1, warmWhite, intensity);
  }
}
void animateNoon(byte maxIntensity) {
  byte lo = 12; 
  byte hi = 15;
  byte intensity = brightness;
  if (lo <= timeOfDay && timeOfDay < hi) {
    intensity = brightnessDescending(lo, hi, timeOfDay, maxIntensity);
    setPixel(ambientRight2, warmWhite, intensity);
  }
  lo = 12; hi = 16;
  if (lo <= timeOfDay && timeOfDay < hi) {
    intensity = brightnessDescending(lo, hi, timeOfDay, maxIntensity);
    setPixel(ambientRight1, warmWhite, intensity);
  }
  lo = 10; hi = 13;
  if (lo <= timeOfDay && timeOfDay < hi) {
    intensity = brightnessAscending(lo, hi, timeOfDay, maxIntensity);
    setPixel(ambientLeft1, warmWhite, intensity);
  }
  lo = 11; hi = 14;
  if (lo <= timeOfDay && timeOfDay < hi) {
    intensity = brightnessAscending(lo, hi, timeOfDay, maxIntensity);
    setPixel(ambientLeft2, warmWhite, intensity);
  }
}
void animateAfternoon(byte maxIntensity) {
  byte lo = 13; 
  byte hi = 16;
  byte intensity = brightness;
  if (lo <= timeOfDay && timeOfDay < hi) {
    intensity = brightnessDescending(lo, hi, timeOfDay, maxIntensity);
    setPixel(ambientLeft1, warmWhite, intensity);
  }
  lo = 14; hi = 17;
  if (lo <= timeOfDay && timeOfDay < hi) {
    intensity = brightnessDescending(lo, hi, timeOfDay, maxIntensity);
    setPixel(ambientLeft2, warmWhite, intensity);
  }
}
void animateEvening(byte maxIntensity) {
  byte lo = 14; 
  byte hi = 16;
  byte intensity = brightness;
  if (lo <= timeOfDay && timeOfDay < hi) {
    intensity = brightnessAscending(lo, hi, timeOfDay, maxIntensity);
    setPixel(ambientLeft3, orange, intensity);
  }
  lo = 16; hi = 17;
  if (lo <= timeOfDay && timeOfDay < hi) {
    intensity = brightnessAscending(lo, hi, timeOfDay, maxIntensity);
    setPixel(ambientLeft4, red, intensity);
  }
}
void animateSunset(byte maxIntensity) {
  byte lo = 16; 
  byte hi = 17;
  byte intensity = brightness;
  if (lo <= timeOfDay && timeOfDay < hi) {
    intensity = brightnessDescending(lo, hi, timeOfDay, maxIntensity);
    setPixel(ambientLeft3, orange, intensity);
  }
  lo = 17; hi = 18;
  if (lo <= timeOfDay && timeOfDay < hi) {
    intensity = brightnessDescending(lo, hi, timeOfDay, min(maxIntensity, 128));
    setPixel(ambientLeft4, red, intensity);
  }
}
void animateSun() {
  byte maxIntensity = 255 - (255 - brightness) * 0.5; // brighter
  animateSunrise(maxIntensity); 
  animateMorning(maxIntensity);
  animateNoon(maxIntensity);
  animateAfternoon(maxIntensity);
  animateEvening(maxIntensity);
  animateSunset(maxIntensity);    
}
void animateHeavens() {
  animateStar();
  if (animatingDay) {
    animateSun();
  } 
}
void animateTree() {
  int elapsed = millis() - time;
  uint32_t color;
  byte GVariation = float(brightness) * 20 / 255;
  byte BVariation = float(brightness) * 10 / 255;
  byte G = green >> 8;
  byte B = green;
  byte intensity = brightness;
  if (brightness > 0.8) {
    intensity -= float(brightness) * 0.2 * abs(sin(PI * elapsed / period3));
  } else {
    intensity += float(brightness) * 0.2 * abs(sin(PI * elapsed / period3));
  }
  
  G -= abs(cos(PI * elapsed / period1)) * GVariation;
  B -= abs(cos(PI * elapsed / period2)) * BVariation;

  color = B;
  color |= (uint32_t(G) << 8);
  
  setPixel(TREE, color, intensity);
}
void animateTV(bool screenTime) {
  if (screenTime == true) {

    uint32_t tvColor = strip.getPixelColor(TV);
    byte intensity = min(maxBrightness, brightness * 1.5);
    
    byte r = random(100);
    if (r < 1) {
      setPixel(TV, strip.Color(random(120), random(170), random(255)), intensity);
    } else {
      strip.setPixelColor(TV, tvColor);
    }
  } else {
    setPixel(TV, 0, 0);
    if (!nightMode) {
      if (animatingDay && 18 < timeOfDay) {
        setPixel(TV, warmWhite, min(brightness, 128));
      }
      if(animatingDay && timeOfDay < 2) {
        setPixel(TV, warmWhite, min(brightness, 128));
      }
    }
  }
}
void animateHouse() {
  bool sunIsToRiseSoon = animateNextDay > now;
  animateTV(sunIsToRiseSoon);
  if (!sunIsShining) {
    setPixel(house[0], warmWhite, min(brightness, 128));
  }
}
void animateCloud() {
  bool lightning = random(4000) < 1 && nightMode == false;
  byte cloudSize = sizeof(cloud);
  uint32_t color = coolWhite;
  float intensity = brightness / 2;
  byte lightningCycles = intensity;
   
  if (lightning) {
    lightningInProgress = true;
    lightningDuration = 0;
  }
  lightning = lightningInProgress && (lightningDuration < lightningCycles);
  if (lightning) {
    color = white;
    intensity = 255;
  } else {
    lightningInProgress = false;
  }
  lightningDuration++;
  if (!sunIsShining) {
    for (byte p = 0; p < cloudSize; p++) {
      setPixel(cloud[p], color, intensity);
    }
  }
}
void setPixel(byte index, uint32_t color, byte luminosity) {

  byte B = float(luminosity) / 255 * byte(color);
  byte G = float(luminosity) / 255 * byte(color >> 8);
  byte R = float(luminosity) / 255 * byte(color >> 16);

  color = B;
  color |= (uint32_t(G) << 8);
  color |= (uint32_t(R) << 16);
  
  strip.setPixelColor(index, color);
}
void getLightLevel(bool exponential) {
  float dimmingFactor;

  byte newBrightness;
  int VLow = 660;
  int VHigh = 30;
  int v = analogRead(LIGHTSENSORPIN);

  if (now - sinceLastSample > brightnessDamping) {
    sinceLastSample = now;

    if (v < VHigh) { v = VHigh; }
    if (v > VLow) { v = VLow; }

    int sensorRange = VLow - VHigh;
    float brightnessRange = float(maxBrightness) - minBrightness;
    float sensorProportion = float(VLow - v) / sensorRange; 
    float intensity = sensorProportion * brightnessRange; 
    
    newBrightness = intensity + minBrightness;
    float ascendingQuadrant = PI * 0.5; 
    dimmingFactor = cos(float(newBrightness) / 255 * ascendingQuadrant) * 255; // cosine easing
    newBrightness = 255 - dimmingFactor;
    
    if (brightness < newBrightness) {
      sensorVectorBalance++;
    } else if (newBrightness < brightness) {
      sensorVectorBalance--;
    }
    if (sensorVectorBalance > sensorCalibration + sensorDampening) {
      lastHi = millis();
      brightness++;
      sensorVectorBalance = sensorCalibration;
    } else if (sensorVectorBalance < sensorCalibration - sensorDampening) {
      unsigned long now = millis();
      int sinceLo = now - lastLo;
      if (gestureMinTime <= sinceLo && sinceLo < gestureTimeout) {
        if (lastHi - lastLo > 500) {
          queueNextDayAnimation();
        }
      }
      lastLo = now;
      brightness--;
      sensorVectorBalance = sensorCalibration;
    }

    brightness = max(minBrightness, brightness);
    brightness = min(maxBrightness, brightness);

    nightMode = brightness <= minBrightness + 5;
    dayMode = brightness >= maxBrightness - 5;

  }
}
void animateSanta() {
  setPixel(SANTA, red, brightness);
}
void setup() {
  pinMode(LIGHTSENSORPIN, INPUT);
  Serial.begin(57600);
  strip.begin();
  strip.show(); 
}
void loop() {
  resetAmbient();
  animateTime();
  getLightLevel(true);
  animateHeavens();
  animateHouse();
  animateSanta();
  animateTree();
  animateReindeer();
  animateCloud();
  strip.show();
}
