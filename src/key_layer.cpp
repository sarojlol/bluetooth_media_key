#include <Arduino.h>
#include <key_layer.h>
#include <pins_define.h>
#include <encoder.h>
#include <lightning.h>
#include <EEprom.h>
#include <BleKeyboard.h>

BleKeyboard Keyboard;

int key_layer;
int last_key_layer;

volatile bool mode_change;

extern volatile long newPosition;
extern long last_pos;

void ble_setup()
{
    Keyboard.setName("Ze bluetooth dewise");
    Keyboard.begin();
}

void start_layer_change()
{
    mode_change = true;
    layer_light(key_layer);
}

bool isConnected(){
    return Keyboard.isConnected();
}

void key_layer_switch()
{
    checkPosition();
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
        EEPROMWritelong(25, key_layer);
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

void mute()
{
    switch (key_layer)
    {
    case 0:
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
        break;
    }
}

void next_track()
{
    switch (key_layer)
    {
    case 0:
        Keyboard.write(KEY_MEDIA_NEXT_TRACK);
        break;
    }
}

void previous_track()
{
    switch (key_layer)
    {
    case 0:
        Keyboard.write(KEY_MEDIA_PREVIOUS_TRACK);
        break;
    }
}

void play_pause()
{
    switch (key_layer)
    {
    case 0:
        Keyboard.write(KEY_MEDIA_PLAY_PAUSE);
        break;
    }
}
