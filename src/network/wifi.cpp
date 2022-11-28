//
// Created by 初音未来 on 2022/11/28.
//

#include "ESP8266WebServer.h"
#include "ESP8266WiFi.h"
#include "wifi.h"

ESP8266WebServer WifiConfigPort(2233);


//字符串转IP地址
IPAddress StrToIP(String str) {
    IPAddress ipadds;
    ipadds.fromString(str);
    return ipadds;
}


void APStart() {
    WiFi.softAPConfig(StrToIP("192.168.39.39"),
                      StrToIP("192.168.39.1"),
                      StrToIP("255.255.255.0"));
    WiFi.softAP("凌Pro-1");
}

// 网页界面
void WRIndex() {
    String ret = String("<html><head><meta charset=\"utf-8\"><title>凌Pro-1</title></head><body>") +
                 String("<center>") + String("<h3>配</h3>") +
                 String("<input name=\"range\" id=\"range\" type=\"range\" min=\"8\" max=\"18\" value=\"8\" oninput=\"change()\"/>") +
                 String("</center><script type=\"text/javascript\">\n"
                        "  function change() {\n"
                        "    var value = document.getElementById(\"range\").value;\n"
                        "    console.log(value);"
                        "var httpRequest = new XMLHttpRequest();//第一步：建立所需的对象\n"
                        "        httpRequest.open('GET', '/all?range='+value, true);//第二步：打开连接  将请求参数写在url中  ps:\"./Ptest.php?name=test&nameone=testone\"\n"
                        "        httpRequest.send();//第三步：发送请求  将请求参数写在URL中\n"
                        "  }\n"
                        "</script></body></html>");
    WifiConfigPort.send(200, "text/html", ret);
}

void WRAll() {
    String a = WifiConfigPort.arg("range");
    analogWrite(D0, a.toInt());
    WifiConfigPort.send(200, "text/html", "ok");

}


void WFbegin() {
    APStart();
    WifiConfigPort.on("/", WRIndex);
    WifiConfigPort.on("/all", WRAll);
    WifiConfigPort.begin();
}


void wifi::begin() {
    WFbegin();
}

void wifi::loop() {
    if (WiFi.status() != 3) {
        // 循环开启客户端
        WifiConfigPort.handleClient();

    }
}










