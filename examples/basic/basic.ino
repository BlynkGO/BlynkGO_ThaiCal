#include <BlynkGO_ThaiCal.hpp>  /* พ.ศ.2540 - ปีปัจจุบัน*/

#define SSID        "..."
#define PASSWORD    "..."

void setup() {
  Serial.begin(115200); Serial.println();

  WiFi.begin(SSID, PASSWORD);
  while(!WiFi.isConnected())delay(400);
  Serial.println(WiFi.localIP());

  bool is_wanpra;
  String thai_date = BlynkGO_ThaiCal::get(5,1,2024, &is_wanpra); // วัน, เดือน, ปีค.ศ.

  Serial.println(thai_date);
  if(is_wanpra){
    Serial.println("เป็นวันพระ");
  } else {
    Serial.println("ไม่ใช่วันพระ");
  }
}

void loop() {
  
}