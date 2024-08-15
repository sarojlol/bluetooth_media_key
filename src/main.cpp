#include <Arduino.h>
#include <pins_define.h>
#include <BleKeyboard.h>
#include <RotaryEncoder.h>
#include <OneButton.h>
#include <FastLED.h>
#include <EEPROM.h>

void checkPosition();
void mute();
void next_track();
void previous_track();
void play_pause();
void start_layer_change();
void buttonLED_fade();
void start_light_setting();
void light_setting ();
void end_light_setting();
void start_light_change();
void change_mode_light();
void end_light_change();
void light_show(int mode);
void layer_light(int mode);
void key_layer_switch();
void end_layer_change();
void media_layer();

RotaryEncoder *encoder = nullptr;

BleKeyboard Keyboard;

CRGB leds[NUM_LEDS];

OneButton rotary(rotary_button, true);
OneButton next(next_track_button, true);
OneButton previous(previous_track_button, true);
OneButton play(play_pause_button, true);

int light_mode;
int last_light_mode;
int key_layer;
int last_key_layer;
volatile bool mode_change;
long last_pos;
long newPosition;
int animation_speed;
int fixed_hue;
int last_animation_speed;
int last_fixed_hue;

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  // EEPROM.write(1, 2);
  // light_mode = EEPROM.read(0);
  // key_layer = EEPROM.read(1);
  last_light_mode = light_mode;
  last_key_layer = key_layer;
  encoder = new RotaryEncoder(rotary_dt, rotary_clk, RotaryEncoder::LatchMode::TWO03);

  // register interrupt routine
  attachInterrupt(digitalPinToInterrupt(rotary_dt), checkPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(rotary_clk), checkPosition, CHANGE);

  rotary.attachClick(mute);

  next.attachClick(next_track);
  next.attachLongPressStart(start_light_change);
  next.attachDuringLongPress(light_setting);
  next.attachLongPressStop(end_light_setting);

  
  previous.attachClick(previous_track);
  previous.attachLongPressStart(start_layer_change);
  previous.attachDuringLongPress(key_layer_switch);
  previous.attachLongPressStop(end_layer_change);

  play.attachClick(play_pause);
  play.attachDuringLongPress(change_mode_light);
  play.attachLongPressStart(start_light_change);
  play.attachLongPressStop(end_light_change);

  ledcAttachPin(led_pin, 0);
  ledcSetup(0, 100, 10);

  FastLED.addLeds<NEOPIXEL, ws2812_PIN>(leds, NUM_LEDS);
  layer_light(key_layer);
  Serial.print(EEPROM.read(1));

  Keyboard.setName("Ze bluetooth dewise");
  Keyboard.begin();

}

void loop() {
  if (Keyboard.isConnected())
  {
    encoder->tick();
    rotary.tick();
    next.tick();
    previous.tick();
    play.tick();
    newPosition = encoder->getPosition()/2;
    if (!mode_change)
    {
      switch (key_layer)
      {
      case 0:
        media_layer();
        break;
      }
      light_show(light_mode);
    }
  }
  else{
    buttonLED_fade();
  }
}

void checkPosition()
{
  encoder->tick(); // just call tick() to check the state.
}

void mute()
{
  static bool ismute;
  ismute =! ismute;
  if (ismute)
  {
    Keyboard.write(KEY_MEDIA_MUTE);
  }
  else 
  {
    Keyboard.write(KEY_MEDIA_VOLUME_DOWN);
    Keyboard.write(KEY_MEDIA_VOLUME_UP);
  }
}

void next_track()
{
  Keyboard.write(KEY_MEDIA_NEXT_TRACK);
}

void previous_track()
{
  Keyboard.write(KEY_MEDIA_PREVIOUS_TRACK);
}

void play_pause()
{
  Keyboard.write(KEY_MEDIA_PLAY_PAUSE);
}

void buttonLED_fade()
{
  static unsigned long last_delay;
  static int pwm_val;
  static bool pwm_flag;
  if ((millis() - last_delay) > 10)
  {
    if (pwm_val >= 0 && pwm_val <= 1023 &! pwm_flag)
    {
      pwm_val += 10;
      if (pwm_val >= 1023){pwm_flag = true;}
    }
    if (pwm_val <= 1023 && pwm_val >= 0 && pwm_flag)
    {
      pwm_val -= 10;
      if ( pwm_val <= 0){pwm_flag = false;}
    }
    
    pwm_val = constrain(pwm_val, 0, 1023);
    ledcWrite(0, pwm_val);
    last_delay = millis();
  }
}

void start_light_change()
{
  mode_change = true;
}

void change_mode_light()
{
  checkPosition();
  newPosition = encoder->getPosition()/2;
  if (last_pos != newPosition)
  {
    if (newPosition > last_pos)
    {
      light_mode += 1;
    }
    else if (newPosition < last_pos)
    {
      light_mode -= 1;
    }
    light_mode = constrain(light_mode, 0, max_light_mode-1);
    last_pos = newPosition;
  }
  light_show(light_mode);
}

void start_light_setting()
{
  mode_change = true;
}

void light_setting ()
{
  checkPosition();
  newPosition = encoder->getPosition()/2;
  if (last_pos != newPosition)
  {
    static long pos_diff;
    pos_diff = newPosition - last_pos;
    if (light_mode == 0 || light_mode == 1 || light_mode == 2)
    {
      animation_speed = animation_speed + pos_diff;
      animation_speed = constrain(animation_speed, 0, 200);
    }
    else if (light_mode == 3 || light_mode == 4)
    {
      fixed_hue = fixed_hue + pos_diff;
      fixed_hue = constrain(fixed_hue, 0, 255);
    }
    last_pos = newPosition;
  }
  light_show(light_mode);
}

void end_light_setting()
{
  if (last_animation_speed != animation_speed)
  {

  }
  if (last_fixed_hue != fixed_hue)
  {

  }
  mode_change = false;
}

void end_light_change()
{
  if (last_light_mode != light_mode){
    // EEPROM.write(0, light_mode);
    last_light_mode = light_mode;
  }
  mode_change = false;
}

void light_show(int mode)
{
  static unsigned long last_delay;
  static uint8_t hue;
  switch (mode)
  {
  case 0:
    if ((millis() - last_delay) > animation_speed)
    {
      hue -= 1;
      last_delay = millis();
    }
    fill_rainbow(leds, 3, hue, 35);
    FastLED.show();
    break;
  
  case 1:
    if ((millis() - last_delay) > animation_speed)
    {
      hue += 1;
      last_delay = millis();
    }
    fill_rainbow(leds, 3, hue, 35);
    FastLED.show();
    break;
  case 2:
    if ((millis() - last_delay) > animation_speed)
    {
      hue += 1;
      last_delay = millis();
    }
    fill_solid(leds, 3, CHSV(hue, 255, 255));
    FastLED.show();
    break;
  case 3:
    static int saturation;
    static bool pwm_flag;
    if ((millis() - last_delay) > 5)
    {
      if (saturation >= 0 && saturation <= 255 &! pwm_flag)
      {
        saturation += 1;
        if (saturation >= 255){pwm_flag = true;}
      }
      if (saturation <= 255 && saturation >= 0 && pwm_flag)
      {
        saturation -= 1;
        if (saturation <= 0){pwm_flag = false;}
      }
      
      saturation = constrain(saturation, 0, 1023);
      fill_solid(leds, 3, CHSV(fixed_hue, 255, saturation));
      FastLED.show();
      last_delay = millis();
    }
    break;
  case 4:
    fill_solid(leds, 3, CHSV(fixed_hue, 255, 255));
    FastLED.show();
    break;
  case max_light_mode-1:
    fill_solid(leds, 3, CRGB::Black);
    FastLED.show();
    break;
  }
}

void layer_light(int mode)
{
  switch (mode)
  {
  case 0:
    fill_solid(leds, 3, CRGB::Purple);
    FastLED.show();
    break;
  case 1:
    fill_solid(leds, 3, CRGB::Cyan);
    FastLED.show();
    break;
  case 2:
    fill_solid(leds, 3, CRGB::Blue);
    FastLED.show();
    break;
  }
}

void start_layer_change()
{
  mode_change = true;
  layer_light(key_layer);
}

void key_layer_switch()
{
  checkPosition();
  newPosition = encoder->getPosition()/2;
  if (last_pos != newPosition)
  {
    if (newPosition > last_pos)
    {
      key_layer += 1;
    }
    else if (newPosition < last_pos)
    {
      key_layer -= 1;
    }
    key_layer = constrain(key_layer, 0, max_layer-1);
    last_pos = newPosition;
  }
  layer_light(key_layer);
}

void end_layer_change()
{
  if (last_key_layer != key_layer)
  {
    // EEPROM.write(1, key_layer);
    last_key_layer = key_layer;
  }
  mode_change = false;
}

void media_layer()
{
  if (last_pos != newPosition)
  {
    digitalWrite(led_pin, LOW);
    if (newPosition > last_pos)
    {
      Keyboard.write(KEY_MEDIA_VOLUME_UP);
    }
    else if (newPosition < last_pos)
    {
      Keyboard.write(KEY_MEDIA_VOLUME_DOWN);
    }
    last_pos = newPosition;
  }
}