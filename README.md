# Gemma led animations

Just wanted to create a quick template for simple animation of LEDs.  The main data structure is as follows:

```cpp
struct Animation {
  uint8_t nFrames;
  uint8_t repetitions;
  uint16_t frameDuration;
  uint32_t *frames;
  Animation* next;
};
```

__nFrames__: This is the number of frames in the __frames__ data block

__repetitions__: This is the number of repetitions for this animation, 0 repeats forever

__frameDuration__: This is how quickly frames advance one to another in ms.

__frames__: This is the frame data block.  Each LED is represented by a uint32_t in the form of 0x00RRGGBB.

__next__: If there is an animation to play after this one, it's referenced here.

It should be clear from the spec that an animation that has a value of 0 for repetitions, 
or next == null will be the last animtion to be played. The reason for this flexibility is 
to have the potential for an "intro" animation, where leds light up in one way, but then 
repeat in a different way.

For now, the frame data here is hard-coded for an array of 4 leds.  The animation code is 
flexible enough to handle more LEDs, however the animation definitions would have to take
in to account the extra leds.

Gemma is a little special because it has a low amount of memory.  If you run in to issues 
where your animation will not fit, __frames__ can be downscaled to a uint16_t if you don't
need the color fidelity, aka 0xRGB.  The Neopixel takes up ~1800 bytes, the animation code 
on top of that takes up ~2250 bytes.  I welcome any suggestions to help cut the fat here
to help squeeze out any more frames on low memory devices.
