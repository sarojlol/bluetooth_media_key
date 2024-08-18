#include <Arduino.h>
#include <EEprom.h>
#include <Wire.h>
#include <I2C_eeprom.h>
I2C_eeprom ee(0x50, I2C_DEVICESIZE_24LC256);
uint32_t start, dur1, dur2;

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

void eeprom_setup()
{
    Wire.begin();
    ee.begin();
}

long EEPROMReadlong(long address) 
{
    long four = ee.readByte(address);
    long three = ee.readByte(address + 1);
    long two = ee.readByte(address + 2);
    long one = ee.readByte(address + 3);
    
    return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

void EEPROMWritelong(int address, long value) 
{
    byte four = (value & 0xFF);
    byte three = ((value >> 8) & 0xFF);
    byte two = ((value >> 16) & 0xFF);
    byte one = ((value >> 24) & 0xFF);
    
    ee.writeByte(address, four);
    ee.writeByte(address + 1, three);
    ee.writeByte(address + 2, two);
    ee.writeByte(address + 3, one);
}

// void save_data(long rainbow_animation, long hue_animation, long fade_animation, long fixed_hue, long running_animation, long layer, long light_mode)
// {
//     EEPROMWritelong(0, rainbow_animation);
//     EEPROMWritelong(5, hue_animation);
//     EEPROMWritelong(10, fade_animation);
//     EEPROMWritelong(15, running_animation);
//     EEPROMWritelong(20, fixed_hue);
//     EEPROMWritelong(25, layer);
//     EEPROMWritelong(30, light_mode);
// }

void restore_data()
{
    key_layer = EEPROMReadlong(25);
    fixed_hue = EEPROMReadlong(20);
    rainbow_animation = EEPROMReadlong(0);
    fade_animation = EEPROMReadlong(10);
    hue_animation = EEPROMReadlong(5);
    running_animation = EEPROMReadlong(15);
    light_mode = EEPROMReadlong(30);
    last_key_layer = key_layer;
    last_fixed_hue = fixed_hue;
    last_rainbow_animation = rainbow_animation;
    last_hue_animation = hue_animation;
    last_running_animation = running_animation;
    last_fade_animation = fade_animation;
    last_light_mode = light_mode;
}
