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

unsigned long since = 0;
unsigned long lastLo = 0;
unsigned long lastHi = 0;
int gestureTimeout = 3000;
int gestureMinTime = 1000;

unsigned long animateNextDay = 0;

int dayDuration = 10000;
bool animatingDay = false;

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
byte brightnessAscending(byte lo, byte hi, byte val, byte maxBrightness) {
  byte brightness = 0;
  if (lo <= val && val < hi) {
    byte range = (hi - lo);
    byte center = lo + range;
    float proportion = 1 - (float(hi) - val) / range;
    brightness = proportion * maxBrightness;
  }
  return brightness;
}
byte brightnessDescending(byte lo, byte hi, byte val, byte maxBrightness) {
  byte brightness = 0;
  if (lo <= val && val < hi) {
    byte range = (hi - lo);
    byte center = lo + range;
    float proportion = (float(hi) - val) / range;
    brightness = proportion * maxBrightness;
  }
  return brightness;
}
byte brightnessCurve(byte lo, byte hi, byte val, byte maxBrightness) {
  byte brightness = 0;
  if (lo <= val && val < hi) {
    byte range = (hi - lo) / 2;
    byte center = lo + range;
    float proportion = 1 - (float(center) - val) / range;
    brightness = proportion * maxBrightness;
  }
  return brightness;
}
void animateAmbient() {
  uint32_t now = millis();
  int dayTime = now % dayDuration;
  byte hour = dayTime / (dayDuration / 24);
  byte lo;
  byte hi;
  float maxIntensity = 255 - (255 - brightness) * 0.67; // brighter
  byte intensity = maxIntensity;
  animatingDay = animateNextDay <= now && now < animateNextDay + dayDuration;
  resetAmbient();
  if (animatingDay) {
    // fade star against sun animation
    lo = 0; hi = 6;
    if (lo <= hour && hour < hi) {
      animateStar(brightness);
    }
    lo = 6; hi = 10;
    if (lo <= hour && hour < hi) {
      animateStar(brightnessDescending(lo, hi, hour, brightness));
    }
    lo = 15; hi = 18;
    if (lo <= hour && hour < hi) {
      animateStar(brightnessAscending(lo, hi, hour, brightness));
    }
    lo = 18;
    if (lo <= hour) {
      animateStar(brightness);
    }
    
    // day
    lo = 6; hi = 8;
    if (lo <= hour && hour < hi) {
      intensity = brightnessCurve(lo, hi, hour, maxIntensity);
      setPixel(ambientRight4, red, intensity);
    }
    lo = 7; hi = 10;
    if (lo <= hour && hour < hi) {
      intensity = brightnessCurve(lo, hi, hour, maxIntensity);
      setPixel(ambientRight3, orange, intensity);
    }
    lo = 8; hi = 13;
    if (lo <= hour && hour < hi) {
      intensity = brightnessCurve(lo, hi, hour, maxIntensity);
      setPixel(ambientRight2, warmWhite, intensity);
    }
    lo = 8; hi = 14;
    if (lo <= hour && hour < hi) {
      intensity = brightnessCurve(lo, hi, hour, maxIntensity);
      setPixel(ambientRight1, warmWhite, intensity);
    }
    // noon
    lo = 10; hi = 16;
    if (lo <= hour && hour < hi) {
      intensity = brightnessCurve(lo, hi, hour, maxIntensity);
      setPixel(ambientLeft1, warmWhite, intensity);
    }
    lo = 11; hi = 18;
    if (lo <= hour && hour < hi) {
      intensity = brightnessCurve(lo, hi, hour, maxIntensity);
      setPixel(ambientLeft2, warmWhite, intensity);
    }
    lo = 14; hi = 17;
    if (lo <= hour && hour < hi) {
      intensity = brightnessCurve(lo, hi, hour, maxIntensity);
      setPixel(ambientLeft3, orange, intensity);
    }
    lo = 15; hi = 18;
    if (lo <= hour && hour < hi) {
      intensity = brightnessCurve(lo, hi, hour, maxIntensity);
      setPixel(ambientLeft4, red, intensity);
    }
  } else {
    animateStar(brightness);
  }
  animateHouse(animateNextDay > now);
}
void animateStar(byte intensity) {
  uint32_t color = coolWhite;
  if (random(30) < 1) {
    color = warmWhite;
  }
  setPixel(STAR, color, intensity);
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
void animateHouse(bool screenTime) {
  byte houseSize = sizeof(house);
  if (screenTime == true) {
    uint32_t tvColor = strip.getPixelColor(TV);
    byte intensity = brightness * 1.5;
    if (intensity > maxBrightness) {
      intensity = maxBrightness;
    }
    byte r = random(100);
    if (r < 1) {
      byte r = random(120);
      byte g = random(170);
      byte b = random(255);
      setPixel(TV, strip.Color(r, g, b), intensity);
    } else {
      strip.setPixelColor(TV, tvColor);
    }
  } else {
    setPixel(TV, warmWhite, brightness);
  }
  setPixel(house[0], warmWhite, brightness);
}
void animateCloud() {
  bool tooLateForFlashes = brightness > minBrightness + 5;
  bool lightning = random(7000) < 1 && tooLateForFlashes;
  byte cloudSize = sizeof(cloud);
  byte lightningCycles = brightness / 2;
  uint32_t color = coolWhite;
  float intensity = brightness / 2;
   
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
  for (byte p = 0; p < cloudSize; p++) {
    setPixel(cloud[p], color, intensity);
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

  byte nextValue;
  int VLow = 660;
  int VHigh = 30;
  int v = analogRead(LIGHTSENSORPIN);
  
  if (v < VHigh) { v = VHigh; }
  if (v > VLow) { v = VLow; }
  
  float a = float(VLow - v) / (VLow - VHigh); // proportional value in sensing range
  float b = float(maxBrightness - minBrightness) * a; // normalised proportional brightness
  
  nextValue = b + minBrightness; // proportional brightness
  
  dimmingFactor = cos(float(nextValue) / 255 * 0.5 * PI) * 255; // eased to grow brighter slowly in beginning
  nextValue = 255 - dimmingFactor;
  
  if (nextValue > brightness) {
    sensorVectorBalance++;
  } else if (brightness > nextValue) {
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
  
  if (brightness < minBrightness) brightness = minBrightness;
  if (brightness > maxBrightness) brightness = maxBrightness;
}
void setup() {
  pinMode(LIGHTSENSORPIN, INPUT);
  Serial.begin(57600);
  strip.begin();
  strip.show(); 
}
void loop() {
  if (millis() - since > brightnessDamping) {
    since = millis();
    getLightLevel(true);
  }
  animateAmbient();
  setPixel(SANTA, red, brightness);
  animateTree();
  animateReindeer();
  animateCloud();
  strip.show();
}
