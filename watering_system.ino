#include "virtuabotixRTC.h" // Thư viện cho DS1302
#include "avr/io.h"
//#include <util/delay.h>
#include "avr/sleep.h"
#include "avr/interrupt.h"
#define sbi(sfr, bit) (sfr |= (1 << bit))
#define cbi(sfr, bit) (sfr &= ~(1 << bit))
int value, percent;                                                              // biến dùng cho cảm biến độ ẩm đất
bool first_time = true, f_wdt = true;
byte bom = 3;
virtuabotixRTC myRTC(4, 1, 0);                                                  // Khởi tạo các chân của DS1302
void setup_watchdog(int ii);                                                    // CLK = P4; I/0 = P1; RESET = P0
void setup() {
  setup_watchdog(8);
  pinMode(A1, INPUT);
  pinMode(bom, OUTPUT);                                              // kiểm tra độ ẩm
  digitalWrite(bom, HIGH);                              
  myRTC.setDS1302Time(52, 59, 7, 7, 1, 1, 2022);   // set up thời gian
}
void loop() {
  myRTC.updateTime();
  /*
  if (myRTC.hours % 8 == 0) 
    {
        value = analogRead(A1);    
       // đọc cảm biến độ ẩm đất
        percent = map(value, 0, 1023, 0, 100);                                  // Chuyển từ giá trị Analog sang % độ ẩm
        while (percent < 50) {                                                // kiểm tra độ ẩm
          digitalWrite(bom, HIGH);   
          delay(1000);// bật máy bơm
          value = analogRead(A1);                                               // đọc cảm biến độ ẩm đất
          percent = map(value, 0, 1023, 0, 100);
      }
      digitalWrite(bom, LOW);
   } }*/

   if (f_wdt) {
   f_wdt = 0;
    myRTC.updateTime();                                                        // Update thời gian 
    if (myRTC.hours % 8 == 0) {                                                 // 8 - 16 - 24 thì xét
      if (first_time) {                                               // kiểm tra độ ẩm
       value = analogRead(A1);    
       // đọc cảm biến độ ẩm đất
        percent = map(value, 0, 1023, 0, 100);                                  // Chuyển từ giá trị Analog sang % độ ẩm
        while (percent < 33) {                                                // kiểm tra độ ẩm
          digitalWrite(bom, LOW);   
          delay(1000);// bật máy bơm
          value = analogRead(A1);                                               // đọc cảm biến độ ẩm đất
          percent = map(value, 0, 1023, 0, 100);    
//           Serial.println(percent); // In                               // Chuyển từ giá trị Analog sang % độ ẩm
        }                                                       
        digitalWrite(bom, HIGH);                                                // tắt máy bơm
        first_time = false;
      }
    }
    else first_time = true;
    system_sleep();
   }
}
void system_sleep() {
  
  cbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter OFF

  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();

  sleep_mode();                        // System actually sleeps here

  sleep_disable();                     // System continues execution here when watchdog timed out 
  
  sbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter ON
  
}

void setup_watchdog(int ii) {
  byte bb;
  int ww;
  if (ii > 9 ) ii=9;
  bb=ii & 7;
  if (ii > 7) bb|= (1<<5);
  bb|= (1<<WDCE);
  ww=bb;

  MCUSR &= ~(1<<WDRF);
  // start timed sequence
   WDTCR |= (1<<WDCE) | (1<<WDE);
  // set new watchdog timeout value
   WDTCR = bb;
   WDTCR |= _BV(WDIE);
}
  
// Watchdog Interrupt Service / is executed when watchdog timed out
ISR(WDT_vect) {
  f_wdt=1;  // set global flag
}










  /*Serial.print("Current Date / Time: ");
  Serial.print(myRTC.dayofmonth); // In ngày (tháng) (khác với tuần)
  Serial.print("/");
  Serial.print(myRTC.month); // In tháng
  Serial.print("/");
  Serial.print(myRTC.year); // In năm
  Serial.print(" ");
  Serial.print(myRTC.hours); // In giờ
  Serial.print(":");
  Serial.print(myRTC.minutes); // In phút
  Serial.print(":");
  Serial.println(myRTC.seconds); // In giây*/
  
  /*Serial.print(percent);
  Serial.print('%');
  Serial.print(" ");
  Serial.print("Gia tri analog: ");
  Serial.print(value);
  Serial.print(" ");
  Serial.print("Gia tri digital: ");
  Serial.println(digitalRead(2));
  delay(1000);
  real_value=0;*/
