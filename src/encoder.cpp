#include <Arduino.h>
#include <pins_define.h>
#include <RotaryEncoder.h>

RotaryEncoder *encoder = nullptr;

volatile long newPosition;

void encoder_setup()
{
    encoder = new RotaryEncoder(rotary_dt, rotary_clk, RotaryEncoder::LatchMode::TWO03);
}

void checkPosition()
{
    encoder->tick(); // just call tick() to check the state.
    newPosition = encoder->getPosition()/2;
}