#include <Adafruit_NeoPixel.h>
#include "anim.h"

#define PIN 1
#define NLEDS 4

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NLEDS, PIN, NEO_GRB + NEO_KHZ800);

long startTime;
Animation *currentAnimation = &pulseRight;
uint32_t transitionFrame[NLEDS];

void setup() {
  strip.begin();
  strip.setBrightness(16);
  strip.show();

  // Sanity tests, check those LEDs
  colorWipe(0xFF0000, 50);
  delay(200);
  colorWipe(0x00FF00, 50);
  delay(200);
  colorWipe(0x0000FF, 50);
  delay(200);
  colorWipe(0xFFFFFF, 50);
  delay(200);
  colorWipe(0, 50);
  delay(500);

  startTime = millis();
}

void loop() {
  // Current wall clock
  long loopTime = millis();

  // Current time within animation
  long animationTime = loopTime - startTime;

  if(animationTime < 0) {
    // Rollover glitch
    animationTime == 0;
    startTime == 0;
  }

  uint32_t* from;
  uint32_t* to;

  Animation *anim = currentAnimation;

  // Nothing to do, just idle...
  if (!anim) {
    delay(1000);
    return;
  }

  // How long this animation should last
  uint32_t desiredLength = anim->frameDuration * anim->nFrames * anim->repetitions;

  // If not looping forever, check to see if we've run out of time
  if (anim->repetitions > 0) {
    // Skip to next animation while there's more left
    while (anim->next && animationTime > desiredLength) {
      animationTime -= desiredLength;
      anim = anim->next;
      setTransitionFrame();
      desiredLength = anim->frameDuration * anim->nFrames * anim->repetitions;
    }
  }

  if (anim->repetitions > 0 &&
      animationTime > desiredLength &&
      !anim->next) {
    // We've run out of repetitions, end on the last note;
    from = to = &anim->frames[anim->nFrames - 1];
    currentAnimation = NULL;

  } else if (animationTime < anim->frameDuration) {
    // Transitioning to first frame

    to = &anim->frames[0];
    from = transitionFrame;

  } else {
    // Normal animation frames

    int toIndex = (int)(animationTime / anim->frameDuration) % anim->nFrames;
    int fromIndex = (toIndex + anim->nFrames - 1) % anim->nFrames;

    to = &anim->frames[toIndex * NLEDS];
    from = &anim->frames[fromIndex * NLEDS];
  }

  // The time between from & to
  int intervalTime = animationTime % anim->frameDuration;

  // Set the pixel colors, mapping between from & to;
  for (int i = 0; i < NLEDS; i++) {
    strip.setPixelColor(i, colorMap(intervalTime, 0, anim->frameDuration, from[i], to[i]));
  }

  strip.show();
}

uint32_t colorMap(long t, long start_t, long end_t, uint32_t start_color, uint32_t end_color) {
  byte start_b = start_color & 0xff; start_color >>= 8;
  byte start_g = start_color & 0xff; start_color >>= 8;
  byte start_r = start_color & 0xff;

  byte end_b = end_color & 0xff; end_color >>= 8;
  byte end_g = end_color & 0xff; end_color >>= 8;
  byte end_r = end_color & 0xff;

  return strip.Color(
           map(t, start_t, end_t, start_r, end_r),
           map(t, start_t, end_t, start_g, end_g),
           map(t, start_t, end_t, start_b, end_b));
}

void setTransitionFrame() {
  for (int i = 0; i < NLEDS; i++)
    transitionFrame[i] = strip.getPixelColor(i);
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}
