#pragma once
#include <avr/io.h>

void setPinOutput(uint8_t);
void setPinInput(uint8_t);
void setPin(uint8_t, bool);
bool readPin(uint8_t);


