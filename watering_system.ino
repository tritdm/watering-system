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
void setup() 
{
    setup_watchdog(8);
    pinMode(A1, INPUT);
    pinMode(bom, OUTPUT);                                              // kiểm tra độ ẩm
    digitalWrite(bom, HIGH);                              
    myRTC.setDS1302Time(52, 59, 7, 7, 1, 1, 2022);   // set up thời gian
}
void loop() 
{
    myRTC.updateTime();
    /*
    if (myRTC.hours % 8 == 0) 
    {
        value = analogRead(A1);                                                    
        percent = map(value, 0, 1023, 0, 100);                                    
        while (percent < 50)                                                      
        {                                                
            digitalWrite(bom, HIGH);   
            delay(1000);// bật máy bơm
            value = analogRead(A1);                                               
            percent = map(value, 0, 1023, 0, 100);                                
        }
        digitalWrite(bom, LOW);
     } 
     */                                                                             /* program without sleep */
    if (f_wdt) 
    {
        f_wdt = 0;
        myRTC.updateTime();                                                         /* time update */
        if (myRTC.hours % 8 == 0)                                                   /* 0h - 8h - 16h check */
        {                                                 
            if (first_time)                                                         // kiểm tra độ ẩm
            {                                               
                value = analogRead(A1);                                             /* read soil humidity sensor */
                percent = map(value, 0, 1023, 0, 100);                              /* convert from analog value to percent */
                while (percent < 33)                                                /* compare humidity percent with the threshold */
                {                                                
                    digitalWrite(bom, LOW);                                         /* start up the water pump */
                    delay(1000); 
                    value = analogRead(A1);                                         
                    percent = map(value, 0, 1023, 0, 100);    
                    /* Serial.println(percent); */                                
                 }                                                       
                 digitalWrite(bom, HIGH);                                           /* shut down the water pump */
                 first_time = false;
            }
        }
        else first_time = true;
        system_sleep();
    }
}
void system_sleep() 
{
    cbi(ADCSRA,ADEN);                                                               /* turn off ADC */

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);                                            /* sleep mode is set here */
    sleep_enable();

    sleep_mode();                                                                   /* system actually sleeps here */

    sleep_disable();                                                                /* system continues execution here when watchdog timed out */

    sbi(ADCSRA,ADEN);                                                               /* turn on ADC */
  
}

void setup_watchdog(int ii) 
{
    byte bb;
    int ww;
    if (ii > 9) ii=9;
    bb = ii & 7;
    if (ii > 7) bb |= (1<<5);
    bb |= (1 << WDCE);
    ww = bb;

    MCUSR &= ~(1 << WDRF);                                                           
    WDTCR |= (1 << WDCE) | (1 << WDE);                                              /* start timed sequence */
    WDTCR = bb;                                                                     /* set new watchdog timeout value */
    WDTCR |= _BV(WDIE);
}
  

ISR(WDT_vect)                                                                       /* Watchdog Interrupt Service / is executed when watchdog timed out */
{
    f_wdt=1;                                                                        /* set global flag */
}
