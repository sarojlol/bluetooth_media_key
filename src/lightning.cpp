#include <Arduino.h>
#include <pins_define.h>
#include <EEprom.h>
#include <FastLED.h>
#include <lightning.h>
#include <encoder.h>
#include <key_layer.h>

int light_mode;
int last_light_mode;

extern int key_layer;
extern int last_key_layer;

extern volatile bool mode_change;

long last_pos;
extern volatile long newPosition;

int rainbow_animation;
int last_rainbow_animation;

int fade_animation;
int last_fade_animation;

int fixed_hue;
int last_fixed_hue;

int hue_animation;
int last_hue_animation;

int running_animation;
int last_running_animation;

int rainbow_animationd_change[] = {0, 1};
int hue_change[] = {4, 5, 7, 9, 11};
int running_animation_change[] = {6, 7, 8, 9, 10, 11};
int fade_animation_change[] = {3, 4};
int hue_animation_change[] = {2};


CRGB leds[NUM_LEDS];

void lighning_setup()
{
    ledcAttachPin(led_pin, 0);
    ledcSetup(0, 100, 10);
    FastLED.addLeds<NEOPIXEL, ws2812_PIN>(leds, NUM_LEDS);
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
    if (last_pos != newPosition)
    {
        static long pos_diff;
        pos_diff = newPosition - last_pos;
        for (int i = 0; i < 2; i++)
        {
        if (light_mode == rainbow_animationd_change[i])
        {
            rainbow_animation = rainbow_animation + pos_diff;
            rainbow_animation = constrain(rainbow_animation, 0, 1000);
        }
        }
        for (int i = 0; i < 5; i++)
        {
        if (light_mode == hue_change[i])
        {
            fixed_hue = fixed_hue + pos_diff;
            fixed_hue = constrain(fixed_hue, 0, 255);
        }
        }
        for (int i = 0; i < 6; i++)
        {
        if (light_mode == running_animation_change[i])
        {
            running_animation = running_animation + pos_diff;
            running_animation = constrain(running_animation, 0, 5000);
        }
        }
        for (int i = 0; i < 6; i++)
        {
        if (light_mode == fade_animation_change[i])
        {
            fade_animation = fade_animation + pos_diff;
            fade_animation = constrain(fade_animation, 0, 5000);
        }
        }
        for (int i = 0; i < 6; i++)
        {
        if (light_mode == hue_animation_change[i])
        {
            hue_animation = hue_animation + pos_diff;
            hue_animation = constrain(hue_animation, 0, 2000);
        }
        }
        last_pos = newPosition;
    }
    light_show(light_mode);
}

void end_light_setting()
{
    if (last_rainbow_animation != rainbow_animation)
    {
        EEPROMWritelong(0, rainbow_animation);
        last_rainbow_animation = rainbow_animation;
    }
    if (last_fixed_hue != fixed_hue)
    {
        EEPROMWritelong(20, fixed_hue);
        last_fixed_hue = fixed_hue;
    }
    if (last_hue_animation != hue_animation)
    {
        EEPROMWritelong(5, hue_animation);
        last_hue_animation = hue_animation;
    }
    if (last_fade_animation != fade_animation)
    {
        EEPROMWritelong(10, fade_animation);
        last_fade_animation = fade_animation;
    }
    if (last_running_animation != running_animation)
    {
        EEPROMWritelong(15, running_animation);
        last_running_animation = running_animation;
    }
    mode_change = false;
}

void end_light_change()
{
    if (last_light_mode != light_mode){
        EEPROMWritelong(30, light_mode);
        last_light_mode = light_mode;
    }
    mode_change = false;
}

void light_show(int mode)
{
    static unsigned long last_delay;
    static uint8_t hue;
    static int saturation;
    static bool pwm_flag;
    static int led_index;
    static int last_led_index;
    switch (mode)
    {
        //rainbow
    case 0:
        if ((millis() - last_delay) > rainbow_animation)
        {
        hue -= 1;
        last_delay = millis();
        }
        fill_rainbow(leds, 3, hue, 35);
        FastLED.show();
        break;
        //rainbow but backward
    case 1:
        if ((millis() - last_delay) > rainbow_animation)
        {
        hue += 1;
        last_delay = millis();
        }
        fill_rainbow(leds, 3, hue, 35);
        FastLED.show();
        break;
        //rainbow hue
    case 2:
        if ((millis() - last_delay) > hue_animation)
        {
        hue += 1;
        last_delay = millis();
        }
        fill_solid(leds, 3, CHSV(hue, 255, 255));
        FastLED.show();
        break;
        //rainbow fade
    case 3:
        if ((millis() - last_delay) > fade_animation)
        {
        if (saturation >= 0 && saturation <= 255 &! pwm_flag)
        {
            saturation += 1;
            if (saturation >= 255)
            {
            pwm_flag = true;
            hue += 10;
            }
        }
        if (saturation <= 255 && saturation >= 0 && pwm_flag)
        {
            saturation -= 1;
            if (saturation <= 0)
            {
            pwm_flag = false;
            hue += 10;
            }
        }
        saturation = constrain(saturation, 0, 1023);
        fill_solid(leds, 3, CHSV(hue, 255, saturation));
        FastLED.show();
        last_delay = millis();
        }
        break;
        //fixed hue fade
    case 4:
        if ((millis() - last_delay) > fade_animation)
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
        //solid hue
    case 5:
        fill_solid(leds, 3, CHSV(fixed_hue, 255, 255));
        FastLED.show();
        break;
        //rainbow running back and forth
    case 6:
        if ((millis() - last_delay) > running_animation)
        {
        if (led_index >= 0 && led_index <= 3 &! pwm_flag)
        {
            led_index += 1;
            if (led_index >= 3)
            {
            pwm_flag = true;
            }
        }
        led_index = constrain(led_index, 0, 2);
        if (led_index <= 3 && led_index >= 0 && pwm_flag)
        {
            led_index -= 1;
            if (led_index <= 0)
            {
            pwm_flag = false;
            }
        }
        led_index = constrain(led_index, 0, 2);
        hue += 10;
        leds[led_index-1] = CHSV(hue, 255, 0);
        leds[led_index] = CHSV(hue, 255, 255);
        leds[led_index+1] = CHSV(hue, 255, 0);
        FastLED.show();
        last_delay = millis();
        }
        break;
        //fixed hue running back and forth
    case 7:
            if ((millis() - last_delay) > running_animation)
        {
        if (led_index >= 0 && led_index <= 3 &! pwm_flag)
        {
            led_index += 1;
            if (led_index >= 3)
            {
            pwm_flag = true;
            }
        }
        led_index = constrain(led_index, 0, 2);
        if (led_index <= 3 && led_index >= 0 && pwm_flag)
        {
            led_index -= 1;
            if (led_index <= 0)
            {
            pwm_flag = false;
            }
        }
        led_index = constrain(led_index, 0, 2);
        leds[led_index-1] = CHSV(fixed_hue, 255, 0);
        leds[led_index] = CHSV(fixed_hue, 255, 255);
        leds[led_index+1] = CHSV(fixed_hue, 255, 0);
        FastLED.show();
        last_delay = millis();
        }
        break;
        //rainbow running forward
    case 8:
        if ((millis() - last_delay) > running_animation)
        {
        led_index += 1;
        if (led_index >= 3)
        {
            led_index = 0;
        }
        led_index = constrain(led_index, 0, 2);
        Serial.println(led_index);
        leds[last_led_index] = CHSV(hue, 255, 0);
        leds[led_index] = CHSV(hue, 255, 255);
        last_led_index = led_index;
        FastLED.show();
        last_delay = millis();
        }
        break;
        //fixed hue running forward
    case 9:
        if ((millis() - last_delay) > running_animation)
        {
        led_index += 1;
        if (led_index >= 3)
        {
            led_index = 0;
        }
        led_index = constrain(led_index, 0, 2);
        Serial.println(led_index);
        leds[last_led_index] = CHSV(fixed_hue, 255, 0);
        leds[led_index] = CHSV(fixed_hue, 255, 255);
        last_led_index = led_index;
        FastLED.show();
        last_delay = millis();
        }
        break;
        //rainbow running backward
    case 10:
        if ((millis() - last_delay) > running_animation)
        {
        led_index -= 1;
        if (led_index <= -1)
        {
            led_index = 2;
        }
        led_index = constrain(led_index, 0, 2);
        hue += 10;
        Serial.println(led_index);
        leds[last_led_index] = CHSV(hue, 255, 0);
        leds[led_index] = CHSV(hue, 255, 255);
        last_led_index = led_index;
        FastLED.show();
        last_delay = millis();
        }
        break;
        //fixed hue running backward
    case 11:
        if ((millis() - last_delay) > running_animation)
        {
        led_index -= 1;
        if (led_index <= -1)
        {
            led_index = 2;
        }
        led_index = constrain(led_index, 0, 2);
        hue += 10;
        Serial.println(led_index);
        leds[last_led_index] = CHSV(fixed_hue, 255, 0);
        leds[led_index] = CHSV(fixed_hue, 255, 255);
        last_led_index = led_index;
        FastLED.show();
        last_delay = millis();
        }
        break;
        //turnoff all leds
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