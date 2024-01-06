#include <BlynkGO_ThaiCal.hpp>  /* พ.ศ.2540 - ปีปัจจุบัน*/

#define SSID        "..."
#define PASSWORD    "..."

void setup() {
  Serial.begin(115200); Serial.println();

  WiFi.begin(SSID, PASSWORD);
  while(!WiFi.isConnected())delay(400);
  Serial.println(WiFi.localIP());

  bool is_wanpra; String wan_text; String year_naksatra;

  // String thai_date = BlynkGO_ThaiCal::get(5,1,2024, &is_wanpra, &wan_text, &year_naksatra); // วัน, เดือน, ปีค.ศ.
  String thai_date = BlynkGO_ThaiCal::get(24,2,2024, &is_wanpra, &wan_text, &year_naksatra);   // 24/02/2024 = วันมาฆบูชา ปี 2024

  Serial.println(thai_date);

  if(is_wanpra){
    Serial.println("เป็นวันพระ");
  } else {
    Serial.println("ไม่ใช่วันพระ");
  }

  if( wan_text != "" ){
    Serial.println( wan_text); 
  }

  Serial.println( year_naksatra );

}

void loop() {  
}
