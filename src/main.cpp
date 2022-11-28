#include <Arduino.h>

#include "U8g2lib.h"

#define CLK_PIN D1
#define DATA_PIN D2

void setup() {
// write your initialization code here
    pinMode(CLK_PIN, INPUT);
    pinMode(DATA_PIN, INPUT);
    analogWriteFreq(50);
    analogWriteRange(200);
    analogWrite(D0,8);
    Serial.begin(115200);
    Serial.println("Rotary Encoder KY-040");
}

void loop() {
// write your code here
    static uint16_t state = 0, counter = 8;

    delayMicroseconds(100); // Simulate doing somehing else as well.

    state = (state << 1) | digitalRead(CLK_PIN) | 0xe000;
    if (state == 0xf000)
    {
        state = 0x0000;
        if (digitalRead(DATA_PIN))
        {
            if (counter < 18)
            {
                counter++;
            }
        }
        else
        {
            if (counter > 8)
            {
                counter--;
            }
        }
        analogWrite(D0,counter);
        Serial.println(counter);
    }
}