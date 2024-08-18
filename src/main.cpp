#include <Arduino.h>
#include <pins_define.h>
#include <OneButton.h>
#include <EEprom.h>
#include <lightning.h>
#include <encoder.h>
#include <key_layer.h>

void restore_data();

OneButton rotary(rotary_button, true);
OneButton next(next_track_button, true);
OneButton previous(previous_track_button, true);
OneButton play(play_pause_button, true);

extern int light_mode;
extern int last_light_mode;

extern int key_layer;
extern int last_key_layer;

extern volatile bool mode_change;

extern long last_pos;
extern volatile long newPosition;

extern int rainbow_animation;
extern int last_rainbow_animation;

extern int fade_animation;
extern int last_fade_animation;

extern int fixed_hue;
extern int last_fixed_hue;

extern int hue_animation;
extern int last_hue_animation;

extern int running_animation;
extern int last_running_animation;

void setup() {
  Serial.begin(115200);
  eeprom_setup();
  restore_data();
  last_light_mode = light_mode;
  last_key_layer = key_layer;

  encoder_setup();
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

  lighning_setup();
  layer_light(key_layer);
  ble_setup();
}

void loop() {
  if (isConnected())
  {
    checkPosition(); 
    rotary.tick();
    next.tick();
    previous.tick();
    play.tick();
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
