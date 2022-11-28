
#include <arduino.h>
#include "network/wifi.h"

#include "Ticker.h"

#define CLK_PIN D1
#define DATA_PIN D2


int EncodeInt = 8;

wifi WiFis;
Ticker ticker;


void setup() {
    // 初始化文件管理
    pinMode(CLK_PIN, INPUT);
    pinMode(DATA_PIN, INPUT);
    analogWriteFreq(50);
    analogWriteRange(200);
    analogWrite(D0, 8);
    Serial.begin(9600);
    WiFis.begin();
    // ticker.attach_ms(100, readEncode);

}

void loop() {
    WiFis.loop();

    // 读取旋转编码器数值
    static uint16_t state = 0, counter = 8;

    delayMicroseconds(100); // Simulate doing somehing else as well.

    state = (state << 1) | digitalRead(CLK_PIN) | 0xe000;
    if (state == 0xf000) {
        state = 0x0000;
        if (digitalRead(DATA_PIN)) {
            if (counter < 18) {
                counter++;
            }
        } else {
            if (counter > 8) {
                counter--;
            }
        }
        analogWrite(D0, counter);
        Serial.println(counter);
    }

}

