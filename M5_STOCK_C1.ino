#include <M5Stack.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "time.h"
#include "Free_Fonts.h" 
#include "D2Coding_12px.h"

#define GFXFF 1

struct cryptoC {
  String currency;
  String sign;
  String price;
};

struct cryptoC ccinfo[5];
int flag = 0;
unsigned long t_unix_date;

void setup() {
  M5.begin();

  WiFi.begin("SSID", "AP Password");

  while(WiFi.status() != WL_CONNECTED) {
    M5.Lcd.print(".");
    delay(1000);
  }
}

String gethttp(String URL)
{
  HTTPClient http;
  http.begin(URL); 
  String payload = "";
  char cpayload;
  if(http.GET() ==  HTTP_CODE_OK || http.GET() == HTTP_CODE_MOVED_PERMANENTLY) {
    payload = http.getString();
  }
  http.end();
  return payload;
}

int getBtcPrice(String name, String btc)
{  
  String url = "https://api.coinone.co.kr/ticker/?currency=" + btc + "&format=json";
  char buf[10];  
  StaticJsonDocument<500> doc;
  deserializeJson(doc, gethttp(url));
  if (doc["result"] != "success") return 0;
  
  String unix_time = doc["timestamp"];
  t_unix_date = unix_time.toInt();
  String price = doc["last"];
  String yesterday_price = doc["yesterday_last"];  
  
  if(price.toInt() < 1) sprintf(buf, "%s", price); 
  long lint = price.toInt();
  sprintf(buf, "%d", lint);

  ccinfo[flag].currency = name;
  ccinfo[flag].price = buf;
  ccinfo[flag].sign = (price.toInt() < yesterday_price.toInt())?"-":"+";

  flag++;  
  return 1;
}

void printPrice()
{
  for(int i=0;i<5;i++) {
    M5.Lcd.setTextColor(TFT_YELLOW);
    M5.Lcd.setFreeFont(&D2Coding_12px); 
    M5.Lcd.print(ccinfo[i].currency); 
    M5.Lcd.setFreeFont(FF1);    
    if (ccinfo[i].sign == "+") M5.Lcd.setTextColor(TFT_RED);
    else M5.Lcd.setTextColor(TFT_BLUE);
    M5.Lcd.print(ccinfo[i].sign); 
    M5.Lcd.setTextColor(TFT_WHITE);
    M5.Lcd.println(ccinfo[i].price);
  }  
}

void loop() {    
  M5.Lcd.setTextColor(TFT_YELLOW);
  M5.Lcd.setTextSize(2);
  M5.Lcd.fillRect(0, 0, 320, 30, TFT_BLUE);
  M5.Lcd.drawString("Now Loading..", 0, 0, 1);
  

  getBtcPrice("비트코인 ", "BTC");
  getBtcPrice("이더리움 ", "ETH");
  getBtcPrice("도지코인", "DOGE");
  //getBtcPrice("시바코인", "SHIB");
  getBtcPrice("라이트코인", "LTC");
  getBtcPrice("리플", "XRP");

  flag = 0;
  M5.Lcd.clear(BLACK);
  M5.Lcd.setCursor(0,60);

  //가격 표시
  printPrice();

  //현재 시각
  char now[12];
  time_t t = t_unix_date + (60*60*9); 
  struct tm *localTM; 
  localTM = localtime(&t);
    
  sprintf(now, "%02d/%02d %02d:%02d", localTM->tm_mon + 1, localTM->tm_mday, localTM->tm_hour, localTM->tm_min);

  //시간 표시
  M5.Lcd.setFreeFont(FF1);
  M5.Lcd.fillRect(0, 0, 320, 30, TFT_BLUE); 
  M5.Lcd.drawString(now, 0, 0, 1); 

  delay(60*1000);
}
