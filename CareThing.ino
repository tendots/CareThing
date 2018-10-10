
#include <M5Stack.h>
#include <WiFi.h>
#include <HTTPClient.h>

#include "MTCH6102.h"

#define ADDR 0x25
#define WIFI_SSID "ASUS-home"
#define WIFI_PASSWORD "aL9cmQEs"

#define MILKCOCOA_APP_ID      "seajn0jjqzi"
#define MILKCOCOA_SERVERPORT  1883
#define MILKCOCOA_DATASTORE   "nuno"

const char MQTT_SERVER[] PROGMEM    = MILKCOCOA_APP_ID ".mlkcca.com";
const char MQTT_CLIENTID[] PROGMEM  = __TIME__ MILKCOCOA_APP_ID;

WiFiClient client;

// Init M5Stack screen
void clearScreen(){
    uint8_t font_buf[20][16];
    uint16_t sj_length;
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.print("========== Milkcocoa subscriber ==========\n\n");
}

MTCH6102 mtch = MTCH6102();
int len = 8;

void setup() {
  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

 delay(1000);
 
 byte data;

  // Initialize the M5Stack object
  M5.begin();
  clearScreen();

  mtch.begin();
  mtch.writeRegister(MTCH6102_MODE, MTCH6102_MODE_STANDBY);
  mtch.writeRegister(MTCH6102_NUMBEROFXCHANNELS, 0x10 );
  mtch.writeRegister(MTCH6102_NUMBEROFYCHANNELS, 0x03);//最低3点必要なため
  mtch.writeRegister(MTCH6102_MODE, MTCH6102_MODE_FULL);
  
  mtch.writeRegister(MTCH6102_CMD, 0x20);
  delay(500);
  
  // Set mode to Touch only
  //mtch.writeRegister(MTCH6102_MODE, 0x02);

  data = mtch.readRegister(MTCH6102_MODE);
  Serial.print("MODE: ");
  Serial.println(data,BIN);
  
  data = mtch.readRegister(MTCH6102_NUMBEROFXCHANNELS);
  Serial.print("NUMBEROFXCHANNELS: ");
  Serial.println(data);

  data = mtch.readRegister(MTCH6102_NUMBEROFYCHANNELS);
  Serial.print("NUMBEROFYCHANNELS: ");
  Serial.println(data);
}

void loop() {
 
  char json[100];
  sprintf(json, "{\"temp\": %2.2f , \"humid\": %2.2f }", (float)1, (float)2);
  HTTPClient http;
  Serial.print("[HTTP] begin...\n");
  http.begin("https://ev2-prod-node-red-033172a6-351.herokuapp.com/nuno"); //HTTP
  int httpCode = http.POST(json);
  http.end();
  
  delay(1000);

  M5.Lcd.setCursor(0, 70);
  
  byte data; 
  float sensVals[10]={0,0,0,0,0,0,0,0,0,0};
  
  
  for (int i = 0; i < len; i++) {
     data = mtch.readRegister(MTCH6102_SENSORVALUE_RX0+i);
     
    //Serial.print(data);
    //Serial.print(", ");

    sensVals[i] = data;
    //M5.Lcd.print(data);
    //M5.Lcd.print(", ");
  }

  
  //センサー値を貯めて利用者の動きをチェック
  /*
  for (int i = 0; i<len; i++){
    float prev;
    if(i==0){
       prev = 0;
    }else{
       prev=sensVals[i-1];
    }
    M5.Lcd.drawLine(i*30, 240-(prev/255)*240, (i+1)*30, 240-(sensVals[i]/255)*240, TFT_WHITE);
  }
  delay(100);
  
  for (int i = 0; i<len; i++){
    float prev;
    if(i==0){
       prev = 0;
    }else{
       prev=sensVals[i-1];
    }
    M5.Lcd.drawLine(i*30, 240-(prev/255)*240, (i+1)*30, 240-(sensVals[i]/255)*240, TFT_BLACK);
  }
  
  */ 
  //Serial.println();
  //M5.Lcd.println();
}


