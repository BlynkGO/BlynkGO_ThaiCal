#ifndef __BLYNKGO_THAICAL_HPP__
#define __BLYNKGO_THAICAL_HPP__

#define BLYNKGO_THAICAL_VERSION    "1.0"

#define ERROR_LOWER_Y2540           -2
#define ERROR_HTTP_FAIL             -1
#define IS_WANPRA                   1
#define IS_NOT_WANPRA               0

#include <Arduino.h>
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
#elif defined (ESP32)
  #include <WiFi.h>
  #include <HTTPClient.h>
#endif


/* พ.ศ.2540 - ปีปัจจุบัน */
class BlynkGO_ThaiCal {
  public:
    BlynkGO_ThaiCal() {};

    static String get(uint8_t day /* 1-31*/, uint8_t month /* 1-12 */, uint16_t year /* ei. 2024 */,  bool *is_wanpra=NULL) {
      if(is_wanpra !=NULL) { *is_wanpra = false; }
      String _thai_date = "";
      int16_t _th_year  = year+543;
      if( _th_year < 2540 ) return _thai_date;

      char   _cur_date[10]; snprintf(_cur_date, sizeof(_cur_date), "%04d%02d%02d", year, month, day);
      // Serial.printf( "_cur_date : %s\n", _cur_date);

      HTTPClient http;
      WiFiClientSecure client_ssl;
      client_ssl.setInsecure();

      http.begin(client_ssl, "https://www.myhora.com/calendar/ical/thai.aspx?" + String(_th_year) + ".ical");
      if(http.GET() != 0){
        if(client_ssl.available()){
          while(client_ssl.available()){
            String line_data = client_ssl.readStringUntil('\n'); line_data.trim();
            if(line_data.startsWith("DTSTART;VALUE=DATE:")){
              line_data.replace("DTSTART;VALUE=DATE:",""); line_data.trim();
              // Serial.printf("[DATE] %s\n", line_data.c_str());
              if( line_data == _cur_date) {
                while(client_ssl.available()){
                  String line_data2 = client_ssl.readStringUntil('\n'); line_data2.trim();
                  if(line_data2.startsWith("SUMMARY")) {
                    line_data2.replace("SUMMARY:", ""); line_data.trim();
                    _thai_date = line_data2;
                    break;
                  }
                }
                break;
              }
            }
          }
        }
      }

      // เช็คว่าเป็นวันพระหรือไม่
      if( _thai_date != "" && is_wanpra != NULL) {
        *is_wanpra = false;

        uint8_t th_moon_day           = _thai_date.substring(_thai_date.indexOf(" ")+1).toInt();  // ค่ำ
        bool    is_th_waxing_moon     = _thai_date.startsWith("ขึ้น");                              // ข้างขึ้น หรือไม่
        // Serial.println( is_th_waxing_moon);
        // Serial.println( th_moon_day );
        switch(th_moon_day){
          case 8: case 15: *is_wanpra = true;  break;
          case 14:
            if(!is_th_waxing_moon) {   // ถ้าเป็นข้างแรม
              http.begin(client_ssl, "https://www.myhora.com/calendar/ical/buddha.aspx?" + String(_th_year) + ".ical");
              if(http.GET() != 0){
                if(client_ssl.available()){
                  while(client_ssl.available()){
                    String line_data = client_ssl.readStringUntil('\n'); line_data.trim();
                    if(line_data.startsWith("DTSTART;VALUE=DATE:")){
                      line_data.replace("DTSTART;VALUE=DATE:",""); line_data.trim();
                      // Serial.printf("[DATE] %s\n", line_data.c_str());
                      if( line_data == _cur_date) {
                        *is_wanpra = true;
                      }
                    }
                  }
                }
              }
            }
            break;
        }
      }

      return _thai_date;
    }

    static int8_t is_wanpra(uint8_t day /* 1-31*/, uint8_t month /* 1-12 */, uint16_t year /* ei. 2024 */  ) {
      int16_t _th_year  = year+543;
      if( _th_year < 2540 ) return ERROR_LOWER_Y2540;  // error : ปีพ.ศ.ต่ำว่า 2540

      char   _cur_date[10]; snprintf(_cur_date, sizeof(_cur_date), "%04d%02d%02d", year, month, day);
      
      HTTPClient http;
      WiFiClientSecure client_ssl;
      client_ssl.setInsecure();

      http.begin(client_ssl, "https://www.myhora.com/calendar/ical/buddha.aspx?" + String(_th_year) + ".ical");
      if(http.GET() != 0){
        if(client_ssl.available()){
          while(client_ssl.available()){
            String line_data = client_ssl.readStringUntil('\n'); line_data.trim();
            if(line_data.startsWith("DTSTART;VALUE=DATE:")){
              line_data.replace("DTSTART;VALUE=DATE:",""); line_data.trim();
              // Serial.printf("[DATE] %s\n", line_data.c_str());
              if( line_data == _cur_date) {
                return 1;   // ใช่ วันพระ
              }
            }
          }
          return 0;   // ไม่ใช่วันพระ
        }else{
          return ERROR_HTTP_FAIL;  // error: HTTP FAIL
        }
      }else{
        return ERROR_HTTP_FAIL;  // error: HTTP FAIL
      }
    }
};


#endif // __BLYNKGO_THAICAL_HPP__