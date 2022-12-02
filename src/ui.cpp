//
// Created by 初音未来 on 2022/12/2.
//


#include "U8g2lib.h"
#include "Ticker.h"

#define CLK_PIN D5
#define DATA_PIN D6
#define SW_PIN D7

typedef struct {
    const char *str;
} MList;

typedef struct {
    int counts;
} MVal;

typedef struct {
    byte En;
    byte Press;
    byte Mode;
} Kick;


Kick kick = {0};
MList list[] = {
        {"all"},
        {"left"},
        {"right"}
};
MVal listVal[] = {
        {8},
        {8},
        {8}
};

uint16_t state = 0, counter = 1;
bool a = false;
unsigned long lastButtonPress = 0;
int currentStateCLK;
int lastStateCLK;
int angle; // 旋转的角度
int keyLongPressTick = 100; // 计时时常
int En = 3; // 使能
int clockEn = 0; // 操作锁

String RunMode = "0"; // 运行模式
//open_iconic_all_1x 字体
//uu8g2_font_t0_11_t_all


U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);


void drawUI() {
    u8g2.clearBuffer();
    int listLen = sizeof(list) / sizeof(MList); // 计算出列表的长度
    for (int i = 0; i < listLen; i++) {
        if (listVal[1].counts == listVal[2].counts) {
            // 判断左耳的角度和右耳的角度是否相同，如果相同则让all数值等于左右耳数值
            listVal[0].counts = listVal[1].counts;
        }
        u8g2.drawStr(8, (i + 1) * 9, list[i].str); // 根据列表的长度绘制选择光标
        angle = 9 * (listVal[i].counts - 8); // 初始为90
        // angle = listVal[i].counts - 8;// 初始为10 最大显示为10
        if (i == 0) {

            if (listVal[1].counts == listVal[0].counts && listVal[2].counts == listVal[0].counts) {
                // 如果left和right数值相等则all正常显示
                u8g2.drawStr(40, (i + 1) * 9, String(angle).c_str());
            } else {
                // 如果left和right数值不相等则all显示**
                u8g2.drawStr(40, (i + 1) * 9, "**");
            }
        } else {
            u8g2.drawStr(40, (i + 1) * 9, String(angle).c_str());
        }

    }
    u8g2.drawVLine(52, 0, 32);
    if (clockEn == 0) {
        u8g2.drawStr(0, counter * 9, ">"); // 根据counter绘制选择光标
    }


    // 绘制IP地址
    u8g2.drawStr(54, 9, "192.168.2.3");

    // open_iconic_all_2x
    u8g2.setFont(u8g2_font_open_iconic_all_2x_t);
    u8g2.drawGlyph(54, 30, 0x00CB);
    // 绘制锁
    if (clockEn == 0) {
        // 长按锁定图标
        u8g2.drawGlyph(54, 30, 0x00CB);
    } else {
        u8g2.drawGlyph(54, 30, 0x00Ca);
    }


    // 绘制信号图标
    u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
    u8g2.drawGlyph(120, 9, 0x00FD);


}

void setup() {
    Serial.begin(115200);
    u8g2.begin();
    pinMode(SW_PIN, INPUT_PULLUP);
    lastStateCLK = digitalRead(CLK_PIN);
    analogWriteFreq(50);
    analogWriteRange(200);
    analogWrite(D0, 8);
}


void loop() {
    if (Serial.available() > 0) {
        // 串口修改运行模式
        RunMode = "0";
        while (Serial.available() > 0) {
            RunMode += char(Serial.read());
            delay(2);
        }
        if (RunMode.length() > 0) {

            Serial.println(RunMode.toInt());
        }
    }

    u8g2.setFont(u8g2_font_t0_11_t_all);
    currentStateCLK = digitalRead(CLK_PIN);
    if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {
        if (digitalRead(DATA_PIN) != currentStateCLK) {
            // 正向旋转
            if (counter < 3 && !a && clockEn == 0) {
                // 移动选择光标
                counter++;
            } else if (!a && clockEn == 0) {
                counter = 1;
            }
            if (a && clockEn == 0) {
                // 修改某个菜单的值
                Serial.println(counter);
                if (counter != 1 && clockEn == 0) {
                    if (listVal[counter - 1].counts < 18) {
                        listVal[counter - 1].counts++;
                    }
                } else {
                    if (listVal[0].counts < 18 && clockEn == 0) {
                        listVal[0].counts++;
                        listVal[1].counts = listVal[0].counts;
                        listVal[2].counts = listVal[0].counts;
                    }
                }

            }
        } else {
            // 反向旋转
            if (counter > 1 && !a && clockEn == 0) {
                counter--;
            } else if (!a) {
                counter = 3;
            }
            if (a && clockEn == 0) {
                Serial.println(counter);
                if (counter != 1) {
                    if (listVal[counter - 1].counts > 8) {
                        listVal[counter - 1].counts--;
                    }
                } else {
                    if (listVal[0].counts > 8) {
                        listVal[0].counts--;
                        listVal[1].counts = listVal[0].counts;
                        listVal[2].counts = listVal[0].counts;
                    }
                }
            }
        }

        Serial.print("Direction: ");
        Serial.print(" | Counter: ");
        Serial.println(counter);
        drawUI();
        u8g2.sendBuffer();
    }
    lastStateCLK = currentStateCLK;
    int btnState = digitalRead(SW_PIN);
    if (btnState == LOW) {
        if (millis() - lastButtonPress > 50) {
            // 按下按钮只执行一次
            keyLongPressTick = 500;
            kick.Mode = 0; // 初始化按键模式，默认短按
            En = 1;// 重置使能
            Serial.println("Button pressed!");
        } else {
            if (keyLongPressTick > 0) {
                // 计时器开始计时
                keyLongPressTick--;
                // drawUI();
                // Serial.println(keyLongPressTick);
                // u8g2.drawLine(52, 32, 52, floor(keyLongPressTick));
            } else if (keyLongPressTick == 0) {
                // 计时器归零时判断为长按
                kick.Mode = 1;
            }
        }
        lastButtonPress = millis();
    } else {
        // 未按按钮循环执行
        if (kick.Mode == 1) {
            // 当判断为长按时的操作
            if (En == 1) {
                En = 0;
                Serial.println("长按");
                kick.Press = 1;
                // 设置为长按
                if (clockEn == 0) {
                    clockEn = 1;
                } else {
                    clockEn = 0;
                }

            }

        } else if (kick.Mode == 0 && clockEn == 0) {
            // 当未判断为长按时的操作
            if (En == 1) {
                // 使能，目的是只激活一次该函数，在按钮按下时重置该值
                En = 0;
                Serial.println("短按");
                kick.Press = 0;
                //设置为短按
                if (a) {
                    // 选项选择，短按修改选项值
                    a = false;
                } else {
                    a = true;
                }
            }
        }
    }
    delay(1);
}
