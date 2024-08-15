#include <Arduino.h>
#include <pins_define.h>
#include <BleKeyboard.h>
#include <RotaryEncoder.h>
#include <OneButton.h>

void checkPosition();
void mute();
void next_track();
void previous_track();
void play_pause();

RotaryEncoder *encoder = nullptr;

BleKeyboard Keyboard;

OneButton rotary(rotary_button, true);
OneButton next(next_track_button, true);
OneButton previous(previous_track_button, true);
OneButton play(play_pause_button, true);

void setup() {
  Serial.begin(115200);
  encoder = new RotaryEncoder(rotary_dt, rotary_clk, RotaryEncoder::LatchMode::TWO03);

  // register interrupt routine
  attachInterrupt(digitalPinToInterrupt(rotary_dt), checkPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(rotary_clk), checkPosition, CHANGE);

  rotary.attachClick(mute);
  next.attachClick(next_track);
  previous.attachClick(previous_track);
  play.attachClick(play_pause);

  ledcAttachPin(led_pin, 0);
  ledcSetup(0, 100, 10);

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
    long newPosition = encoder->getPosition()/2;
    static long last_pos;
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
  else{
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
      Serial.println(pwm_val);
      last_delay = millis();
    }
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
