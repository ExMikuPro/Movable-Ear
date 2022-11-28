
#include <arduino.h>
#include "network/wifi.h"
#include "U8g2lib.h"

#include "Ticker.h"

#define CLK_PIN D5
#define DATA_PIN D6


int EncodeInt = 8;
double angle = 90.0;

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/
                                            U8X8_PIN_NONE);  //M0/ESP32/ESP8266/mega2560/Uno/Leonardo


wifi WiFis;


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
    u8g2.begin();
    u8g2.setFont(u8g2_font_t0_11_t_all);
    u8g2.drawStr(0, 32, String(angle).c_str());
    u8g2.sendBuffer();
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
        if (counter == 18) {
            angle = 0;
        } else {
            angle = 90.0 - 9 * (counter - 8);
        }
        u8g2.clearBuffer();
        u8g2.drawStr(0, 32, String(angle).c_str());
        u8g2.drawStr(0, 22, String(counter).c_str());
        u8g2.sendBuffer();
    }

}

