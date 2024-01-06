#ifndef __BLYNKGO_THAICAL_HPP__
#define __BLYNKGO_THAICAL_HPP__

/** Major version number (X.x.x) */
#define THAICAL_VERSION_MAJOR   1
/** Minor version number (x.X.x) */
#define THAICAL_VERSION_MINOR   0
/** Patch version number (x.x.X) */
#define THAICAL_VERSION_PATCH   1

#define THAICAL_VERSION_TEXT    (String(THAICAL_VERSION_MAJOR)+"."+String(THAICAL_VERSION_MINOR)+"."+String(THAICAL_VERSION_PATCH))

/**
 * Macro to convert IDF version number into an integer
 *
 * To be used in comparisons, such as THAICAL_VERSION >= THAICAL_VERSION_VAL(3, 0, 0)
 */
#define THAICAL_VERSION_VAL(major, minor, patch) ((major << 16) | (minor << 8) | (patch))

/**
 * Current IDF version, as an integer
 *
 * To be used in comparisons, such as BLYNKGO_VERSION >= BLYNKGO_VERSION_VAL(3, 0, 0)
 */
#define THAICAL_VERSION  THAICAL_VERSION_VAL( BLYNKGO_VERSION_MAJOR, \
                                              BLYNKGO_VERSION_MINOR, \
                                              BLYNKGO_VERSION_PATCH)



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

    static String get(uint8_t day /* 1-31*/, uint8_t month /* 1-12 */, uint16_t year /* ei. 2024 */,  bool *is_wanpra=NULL, String *wan_text=NULL, String *year_naksatra=NULL) {
      if(is_wanpra !=NULL)      { *is_wanpra      = false;  }
      if(wan_text  !=NULL)      { *wan_text       = "";     }
      if(year_naksatra  !=NULL) { *year_naksatra  = "";     }

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

            // -- sample บรรทัดที่สำคัญ --
            // DTSTART;VALUE=DATE:20240118
            // DESCRIPTION:วันพฤหัสบดี ขึ้น 8 ค่ำ เดือนยี่(๒) ปีเถาะ
            // SUMMARY:ขึ้น 14 ค่ำ เดือนแปด(๘)
            // SUMMARY:ขึ้น 8 ค่ำ เดือนยี่(๒) วันพระ
            // SUMMARY:ขึ้น 15 ค่ำ เดือนแปด(๘) วันอาสาฬหบูชา
            // SUMMARY:แรม 1 ค่ำ เดือนแปด(๘) วันเข้าพรรษา

            if(line_data.startsWith("DTSTART;VALUE=DATE:")){
              line_data.replace("DTSTART;VALUE=DATE:",""); line_data.trim();
              // Serial.printf("[DATE] %s\n", line_data.c_str());
              if( line_data == _cur_date) {
                while(client_ssl.available()){
                  String line_data2 = client_ssl.readStringUntil('\n'); line_data2.trim();
                  if(line_data2.startsWith("SUMMARY")) {
                    line_data2.replace("SUMMARY:", ""); line_data.trim();

                    if(line_data2.indexOf(" วัน") > 0) {
                      _thai_date = line_data2.substring( 0, line_data2.indexOf(" วัน") );
                      String _wan_text = line_data2.substring( line_data2.indexOf(" วัน")+1 );
                      if(wan_text != NULL) *wan_text = _wan_text;
                      if(is_wanpra !=NULL) *is_wanpra =  ( _wan_text != "" && _wan_text != "วันเข้าพรรษา" );
                    }else{
                      _thai_date = line_data2;
                    }
                    break;
                  }

                  if( year_naksatra != NULL ) {
                    if(line_data2.startsWith("DESCRIPTION")) {
                      *year_naksatra = line_data2.substring( line_data2.indexOf(" ปี") + 1 );
                    }
                  }
                }
                break;
              }
            }
          }
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