/*
  Change of Heart — final code
  Pulse sensor → LED animation + sends BPM to Processing for audio playback.

  Connections:
    - Pulse sensor: analog pin A0
    - LED strips (WS2812B): pins 3, 4, 5, 6, 7
    - Built-in LED blinks on each detected heartbeat

  Required libraries:
    - PulseSensorPlayground
    - FastLED
*/

#include <PulseSensorPlayground.h>
#include <FastLED.h>

// ── LED pins ──────────────────────────────────────────────────────────────────
const int ledStripPin1 = 3;
const int ledStripPin2 = 4;
const int ledStripPin3 = 5;
const int ledStripPin4 = 6;
const int ledStripPin5 = 7;

#define NUM_LEDS 30

CRGB leds1[NUM_LEDS];
CRGB leds2[NUM_LEDS];
CRGB leds3[NUM_LEDS];
CRGB leds4[NUM_LEDS];
CRGB leds5[NUM_LEDS];

// ── Pulse sensor ──────────────────────────────────────────────────────────────
const int PulseWire = 0;
const int LED = LED_BUILTIN;
int Threshold = 600;
PulseSensorPlayground pulseSensor;

int currentBPM = 0;

// ── Timing ────────────────────────────────────────────────────────────────────
unsigned long previousPulseMillis = 0;
unsigned long previousLEDMillis   = 0;
const long pulseInterval = 100;  // check sensor every 100ms
const long ledInterval   = 10;   // update LEDs every 10ms

// ── Animation ─────────────────────────────────────────────────────────────────
float animationStep  = 0;
float animationSpeed = 0;

void setup() {
  Serial.begin(9600);  // must match Processing's baud rate

  pulseSensor.analogInput(PulseWire);
  pulseSensor.blinkOnPulse(LED);
  pulseSensor.setThreshold(Threshold);

  if (pulseSensor.begin()) {
    Serial.println("PulseSensor ready");
  }

  FastLED.addLeds<WS2812B, ledStripPin1, GRB>(leds1, NUM_LEDS);
  FastLED.addLeds<WS2812B, ledStripPin2, GRB>(leds2, NUM_LEDS);
  FastLED.addLeds<WS2812B, ledStripPin3, GRB>(leds3, NUM_LEDS);
  FastLED.addLeds<WS2812B, ledStripPin4, GRB>(leds4, NUM_LEDS);
  FastLED.addLeds<WS2812B, ledStripPin5, GRB>(leds5, NUM_LEDS);
}

void loop() {
  unsigned long currentMillis = millis();

  // ── Read pulse sensor ────────────────────────────────────────────────────
  if (currentMillis - previousPulseMillis >= pulseInterval) {
    previousPulseMillis = currentMillis;

    if (pulseSensor.sawStartOfBeat()) {
      int bpm = pulseSensor.getBeatsPerMinute();

      // Only accept realistic BPM values (40–200)
      if (bpm >= 40 && bpm <= 200) {
        currentBPM = bpm;
        // Send BPM to Processing to trigger audio
        Serial.println(currentBPM);
      }
    }
  }

  // ── Animate LEDs ─────────────────────────────────────────────────────────
  if (currentMillis - previousLEDMillis >= ledInterval && currentBPM > 0) {
    previousLEDMillis = currentMillis;

    float beatPeriod = 60000.0 / currentBPM;
    animationSpeed = NUM_LEDS / (beatPeriod / ledInterval);
    animationStep += animationSpeed;

    if (animationStep >= NUM_LEDS) {
      animationStep -= NUM_LEDS;  // wrap around
    }

    animateLEDStrip(leds1);
    animateLEDStrip(leds2);
    animateLEDStrip(leds3);
    animateLEDStrip(leds4);
    animateLEDStrip(leds5);

    FastLED.show();
  }
}

void animateLEDStrip(CRGB* leds) {
  // Turn all LEDs off
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }

  // Light up 3 consecutive LEDs at the current position
  int ledsToLight = 3;
  for (int i = 0; i < ledsToLight; i++) {
    int currentLED = int(animationStep + i) % NUM_LEDS;
    leds[currentLED] = CRGB::Red;
  }

  // Fade out
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].fadeToBlackBy(20);
  }
}
