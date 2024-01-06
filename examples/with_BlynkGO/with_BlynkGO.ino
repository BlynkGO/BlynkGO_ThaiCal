#include <BlynkGOv3.h>
#include <BlynkGO_ThaiCal.hpp>  /* พ.ศ.2540 - ปีปัจจุบัน*/

GWiFiManager wifi_manager;
GLabel  lb_time;

GLabel  lb_date;
GLabel  lb_thai_date;
GLabel  lb_wanpra;

bool is_update_thai_cal = false;

void setup() {
  Serial.begin(9600); Serial.println();
  BlynkGO.begin();
  BlynkGO.rotation(ROTATION_DEFAULT);

  wifi_manager.align(ALIGN_TOP_RIGHT,-10);
  lb_time.align(wifi_manager, ALIGN_LEFT,-10);

  lb_thai_date.font(prasanmit_40, TFT_WHITE);
  lb_date.align(lb_thai_date, ALIGN_TOP);
  lb_wanpra.align(lb_thai_date, ALIGN_BOTTOM);
}

void loop() {
  BlynkGO.update(); 
  
  if(BlynkGO.isNTPSynced() && is_update_thai_cal ){
    bool is_wanpra; String wan_text; String year_naksatra;
    String thai_date  = BlynkGO_ThaiCal::get(day(), month(), year(), &is_wanpra, &wan_text, &year_naksatra);

    if( thai_date != "") {
      lb_thai_date  = thai_date;
      lb_wanpra     = is_wanpra? "วันนี้ วันพระ" : "วันนี้ ไม่ใช่วันพระ";
      is_update_thai_cal = false;
    }
  }
}

// เมื่อซิงค์เวลา NTP สำเร็จ
NTP_SYNCED(){
  is_update_thai_cal = true;
  static GTimer timer;
  timer.setInterval(1000,[](){
    lb_date = StringX::printf("%02d/%02d/%04d", day() , month() , th_year());
    lb_time = StringX::printf("%02d:%02d:%02d", hour(), minute(), second());
    if( hour()==0 && minute()==0 && second() == 0){
      is_update_thai_cal = true;
    }
  },true);
}

